// Include necessary headers
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h> 
#include <string.h>

// CAN header
#include <fcntl.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>

// Dbus header
#include <stdbool.h>
#include <ctype.h>
#include <dbus/dbus.h>

// Multi-thread header
#include <pthread.h>

#define BUFFER_SIZE 3

// CAN configuration
int soc;    // Variable for can socket
typedef struct {
    uint8_t speed;  // Variable to store speed info
    uint8_t rpm;    // Variable to store rpm info
} CANData;  // Define of CAN data sturcture

CANData buffer[BUFFER_SIZE] = {0};    // Data buffer
int bufferIndex = 0;    // Current buffer index
pthread_mutex_t bufferMutex = PTHREAD_MUTEX_INITIALIZER;    // Mutex for the buffer

int open_port(const char *port);    // Function for open socket
void read_port(uint8_t *speed, uint8_t *rpm);   // Function for read CAN socket
int close_port();   // Function for close socket

// DBus configuration
const char *const INTERFACE_NAME = "org.team4.Des02.CarInformation";
const char *const SERVER_BUS_NAME = "org.team4.Des02";
const char *const CLIENT_BUS_NAME = "org.team4.Des02.Client";
const char *const SERVER_OBJECT_PATH_NAME = "/CarInformation";
const char *const CLIENT_OBJECT_PATH_NAME = "/CarInformationClient";

DBusError dbus_error;   // DBusError instance
void print_dbus_error(char *str);   // Function for print error message

// Multi-thread setting
void *readCANThread(void *arg); // Thread function for reading CAN
void * dbusSendThread(void *arg);   // Thread fucntion for sending via DBus

int main (int argc, char **argv)
{
    // Open the CAN port name "can0"
    open_port("can0");

    // Set up the DBus connection
    DBusConnection *conn;
    dbus_error_init(&dbus_error);
    conn = dbus_bus_get(DBUS_BUS_SESSION, &dbus_error);

    // Check for D-Bus connection errors
    if (dbus_error_is_set(&dbus_error))
        print_dbus_error("Connecting D-Bus Error\n");
    if (!conn) exit(1);

    // Create threads for CAN and DBus
    pthread_t canThread, dbusThread;
    pthread_create(&canThread, NULL, readCANThread, NULL);
    pthread_create(&dbusThread, NULL, dbusSendThread, (void *)conn);

    pthread_join(canThread, NULL);
    pthread_join(dbusThread, NULL);

    close_port();
    return 0;
}

// CAN function definitions

/**
 * @brief Opens the CAN port.
 * 
 * @param port The name of the port to be opened.
 * @return Returns 0 on success, -1 on failure.
 */
int open_port(const char *port)
{
    struct ifreq ifr;
    struct sockaddr_can addr;

    // Open the socket
    soc = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (soc < 0) 
    {
        printf("Error opening socket!");
        return (-1);
    }

    addr.can_family = AF_CAN;
    strcpy(ifr.ifr_name, port);

    // Retrieve the interface index for the interface name
    if (ioctl(soc, SIOCGIFINDEX, &ifr) < 0) 
    {
        printf("Error retrieving interface index!");
        return (-1);
    }

    addr.can_ifindex = ifr.ifr_ifindex;

    // Set the socket to non-blocking mode
    fcntl(soc, F_SETFL, O_NONBLOCK);

    // Bind the socket to the CAN interface
    if (bind(soc, (struct sockaddr *)&addr, sizeof(addr)) < 0) 
    {
        printf("Error binding to the socket!");
        return (-1);
    }

    return 0;
}

/**
 * @brief Reads data from the CAN port.
 * 
 * @param speed Pointer to a variable where the speed data will be stored.
 * @param rpm Pointer to a variable where the RPM data will be stored.
 */
void read_port(uint8_t *speed, uint8_t *rpm)
{
    struct can_frame frame;
    frame.can_dlc = 2;  // Expected Data Length Code
    int recvbytes = 0;

    struct timeval timeout = {1, 0};
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(soc, &readSet);

    // Wait for data to be ready to read
    if (select((soc + 1), &readSet, NULL, NULL, &timeout) >= 0)
    {
        if (FD_ISSET(soc, &readSet))
        {
            recvbytes = read(soc, &frame, sizeof(struct can_frame));
    
            if (recvbytes)
            {
                // Process the CAN frame data to retrieve RPM and Speed
                int disk_rpm = (frame.data[0] << 8) + frame.data[1];
                *rpm = (uint8_t)((float)disk_rpm / 2.6);
                *speed = (uint8_t)(*rpm * 3.4);
            }
        }
    }
}

/**
 * @brief Closes the CAN port.
 * 
 * @return Returns 0 on success.
 */
int close_port()
{
    close(soc);
    return 0;
}


// Dbus function

void print_dbus_error (char *str) 
{
    fprintf (stderr, "%s: %s\\n", str, dbus_error.message);
    dbus_error_free (&dbus_error);
}

// Thread function that reads and saves CAN data 
// This thread operates independently of dbusSendThread
void *readCANThread(void *arg) {
    while (1) {
        // Retrieve speed and rpm values from CAN
        uint8_t speed_value, rpm_value;
        read_port(&speed_value, &rpm_value);

        // Uncomment below to print the retrieved CAN data
        // printf("CAN Data - Speed: %d RPM: %d\n", speed_value, rpm_value);

        // Manage bufferIndex as a circular queue (ring buffer)
        // Use mutex to ensure thread safety during access
        pthread_mutex_lock(&bufferMutex);
        buffer[bufferIndex].speed = speed_value;
        buffer[bufferIndex].rpm = rpm_value;
        bufferIndex = (bufferIndex + 1) % BUFFER_SIZE;
        pthread_mutex_unlock(&bufferMutex);
    }
    return NULL;
}

// Thread function to send CAN data over DBus
// This thread operates independently of readCANThread
void *dbusSendThread(void *arg) {
    // Basic DBus setup
    // Using Thread for dbus connection instance
    DBusConnection *conn = (DBusConnection *)arg;
    DBusMessage *speed_msg, *rpm_msg;
    DBusMessage *speed_req, *rpm_req;
    DBusMessageIter speed_args, rpm_args;
    DBusPendingCall *speed_pending, *rpm_pending;

    while (1) {
        // Ensure thread safety using a mutex
        pthread_mutex_lock(&bufferMutex);

        printf("Current buffer content:\n");
        for (int i = 0; i < BUFFER_SIZE; i++) {
            printf("Buffer[%d] - Speed: %d RPM: %d\n", i, buffer[i].speed, buffer[i].rpm);
        }

        // Get the most recent data index
        int currentIndex = bufferIndex - 1;

        // If bufferIndex is 0, set currentIndex to the last index of the buffer
        if (bufferIndex < 1) 
        {
            currentIndex = BUFFER_SIZE - 1;
        }

        // Store the most recent data from the buffer
        uint8_t speed_value = buffer[currentIndex].speed;
        uint8_t rpm_value = buffer[currentIndex].rpm;

        // Unlock the mutex
        pthread_mutex_unlock(&bufferMutex);

        // Uncomment below to print the CAN data being sent
        // printf("CAN Data - Speed: %d RPM: %d\n", speed_value, rpm_value);

        // Send speed_value to the server using DBus
        // Use "dbus_message_new_method_call" to create a DBus message
        speed_msg = dbus_message_new_method_call(SERVER_BUS_NAME, SERVER_OBJECT_PATH_NAME, INTERFACE_NAME, "setSpeed");
        if (speed_msg == NULL) 
        { 
            fprintf(stderr, "Failed to create Speed Message\n");
            exit(1);
        }

        // Initialize the message to append arguments
        dbus_message_iter_init_append(speed_msg, &speed_args);

        // Attach the actual data (speed_value) as an argument to the message
        if (!dbus_message_iter_append_basic(&speed_args, DBUS_TYPE_BYTE, &speed_value)) 
        {
            fprintf(stderr, "Out of memory when setting Speed!\n");
            exit(1);
        }
        
        // Send the message and await a reply, binding the message with the connection
        if (!dbus_connection_send_with_reply(conn, speed_msg, &speed_pending, -1)) 
        { 
            fprintf(stderr, "Out of memory when sending Speed message!\n"); 
            exit(1);
        }

        // Check if the call is successful
        if (speed_pending == NULL) 
        { 
            fprintf(stderr, "Speed Pending Call failed\n"); 
            exit(1);
        }

        // Flush the connection to ensure all changes are immediately sent to D-Bus
        dbus_connection_flush(conn);
        // Release memory associated with the message
        dbus_message_unref(speed_msg);

        // Block until the pending return is ready, then process the reply
        dbus_pending_call_block(speed_pending);

        // Get the reply message
        speed_req = dbus_pending_call_steal_reply(speed_pending);
        if (speed_req == NULL) 
        {
            fprintf(stderr, "Failed to get Speed Reply\n");
            exit(1);
        }
        dbus_pending_call_unref(speed_pending);

        // Extract arguments from the reply and print them
        char *speed_reply_msg;
        if (dbus_message_get_args(speed_req, &dbus_error, DBUS_TYPE_STRING, &speed_reply_msg, DBUS_TYPE_INVALID)) 
        {
            printf("setSpeed Reply: %s\n", speed_reply_msg);
        }
        else 
        {
             fprintf (stderr, "Failed to get arguments in reply\n");
             exit (1);
        }
        dbus_message_unref(speed_req);

        /*******************************************************************************************************************/

        // Send rpm_value to the server using DBus, following the same procedure as speed_value
        rpm_msg = dbus_message_new_method_call(SERVER_BUS_NAME, SERVER_OBJECT_PATH_NAME, INTERFACE_NAME, "setRpm");
        if (NULL == rpm_msg) 
        { 
            fprintf(stderr, "Failed to create RPM Message\n");
            exit(1);
        }

        dbus_message_iter_init_append(rpm_msg, &rpm_args);
        if (!dbus_message_iter_append_basic(&rpm_args, DBUS_TYPE_BYTE, &rpm_value)) 
        {
            fprintf(stderr, "Out of memory when setting RPM!\n");
            exit(1);
        }

        if (!dbus_connection_send_with_reply(conn, rpm_msg, &rpm_pending, -1)) 
        { 
            fprintf(stderr, "Out of memory when sending RPM message!\n"); 
            exit(1);
        }

        if (NULL == rpm_pending) { 
            fprintf(stderr, "RPM Pending Call failed\n"); 
            exit(1);
        }

        dbus_connection_flush(conn);
        dbus_message_unref(rpm_msg);

        dbus_pending_call_block(rpm_pending);
        rpm_req = dbus_pending_call_steal_reply(rpm_pending);
        if (NULL == rpm_req) 
        {
            fprintf(stderr, "Failed to get RPM Reply\n");
            exit(1);
        }
        dbus_pending_call_unref(rpm_pending);

        char *rpm_reply_msg;
        if (dbus_message_get_args(rpm_req, &dbus_error, DBUS_TYPE_STRING, &rpm_reply_msg, DBUS_TYPE_INVALID)) 
        {
            printf("setRpm Reply: %s\n", rpm_reply_msg);
        }        
        else 
        {
            fprintf (stderr, "Failed to get arguments in reply\n");
            exit (1);
        }
        dbus_message_unref(rpm_req);

        usleep(500000);
    }
    return NULL;
}



