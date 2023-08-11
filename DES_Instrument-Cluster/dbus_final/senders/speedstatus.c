
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

#define BUFFER_SIZE 10

// CAN setting
int soc;
typedef struct {
    uint8_t speed;
    uint8_t rpm;
} CANData;

CANData buffer[BUFFER_SIZE];
int bufferIndex = 0;
pthread_mutex_t bufferMutex = PTHREAD_MUTEX_INITIALIZER;

int open_port(const char *port);
void read_port(uint8_t *speed, uint8_t *rpm);
int close_port();

// Dbus setting
const char *const INTERFACE_NAME = "org.team4.Des02.CarInformation";
const char *const SERVER_BUS_NAME = "org.team4.Des02";
const char *const CLIENT_BUS_NAME = "org.team4.Des02.Client";
const char *const SERVER_OBJECT_PATH_NAME = "/CarInformation";
const char *const CLIENT_OBJECT_PATH_NAME = "/CarInformationClient";

DBusError dbus_error;
void print_dbus_error(char *str);

// Multi-thread setting
void *readCANThread(void *arg);
void * dbusSendThread(void *arg);

int main (int argc, char **argv)
{
    open_port("can0");

    DBusConnection *conn;
    dbus_error_init(&dbus_error);
    conn = dbus_bus_get(DBUS_BUS_SESSION, &dbus_error);

    if (dbus_error_is_set(&dbus_error))
        print_dbus_error("Connecting D-Bus Error\n");

    if (!conn) exit(1);

    pthread_t canThread, dbusThread;
    pthread_create(&canThread, NULL, readCANThread, NULL);
    pthread_create(&dbusThread, NULL, dbusSendThread, (void *)conn);

    pthread_join(canThread, NULL);
    pthread_join(dbusThread, NULL);

    close_port();
    return 0;
}



// CAN function
int open_port(const char *port)
{
    struct ifreq ifr;
    struct sockaddr_can addr;

    /* open socket */
    soc = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (soc < 0) 
    {
	    printf("error!");
        return (-1);
    }

    addr.can_family = AF_CAN;
    strcpy(ifr.ifr_name, port);

    if (ioctl(soc, SIOCGIFINDEX, &ifr) < 0) 
    {
	    printf("error!");
        return (-1);
    }

    addr.can_ifindex = ifr.ifr_ifindex;
    fcntl(soc, F_SETFL, O_NONBLOCK);

    if (bind(soc, (struct sockaddr *)&addr, sizeof(addr)) < 0) 
    {
	    printf("binding error!");
        return (-1);
    }

    return 0;
}


void read_port(uint8_t *speed, uint8_t *rpm)
{
    struct can_frame frame;
    frame.can_dlc = 2;
    int recvbytes = 0;

    struct timeval timeout = {1, 0};
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(soc, &readSet);
    if (select((soc + 1), &readSet, NULL, NULL, &timeout) >= 0)
    {
        if (FD_ISSET(soc, &readSet))
        {
            recvbytes = read(soc, &frame, sizeof(struct can_frame));
            
            // printf("Received bytes: %d\n", recvbytes);
            // printf("Raw data: %x %x\n", frame.data[0], frame.data[1]);
            if (recvbytes)
            {
                int disk_rpm = (frame.data[0] << 8) + frame.data[1];
                *rpm = (uint8_t)((float)disk_rpm / 2.6);
                *speed = (uint8_t)(*rpm * 0.0034);
            }
        }
    }
}


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


void *readCANThread(void *arg) {
    while (1) {
        uint8_t speed_value, rpm_value;
        read_port(&speed_value, &rpm_value);

        printf("CAN Data - Speed: %d RPM: %d\n", speed_value, rpm_value);

        pthread_mutex_lock(&bufferMutex);
        if (bufferIndex < BUFFER_SIZE) {
            buffer[bufferIndex].speed = speed_value;
            buffer[bufferIndex].rpm = rpm_value;
            bufferIndex++;
        }
        pthread_mutex_unlock(&bufferMutex);
    }
    return NULL;
}

void *dbusSendThread(void *arg) {
    DBusConnection *conn = (DBusConnection *)arg;
    DBusMessage *msg;
    DBusMessageIter args;
    DBusPendingCall *pending;

    while (1) {
        pthread_mutex_lock(&bufferMutex);
        if (bufferIndex > 0) {
            uint8_t speed_value = buffer[bufferIndex - 1].speed;
            uint8_t rpm_value = buffer[bufferIndex - 1].rpm;
            bufferIndex--;
            pthread_mutex_unlock(&bufferMutex);

            // Create a new method call and check for errors
            msg = dbus_message_new_method_call(CLIENT_BUS_NAME, CLIENT_OBJECT_PATH_NAME, INTERFACE_NAME, "UpdateCarInfo");
            if (NULL == msg) { 
                fprintf(stderr, "Message Null\n");
                exit(1);
            }

            // Append arguments onto signal
            dbus_message_iter_init_append(msg, &args);
            if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_BYTE, &speed_value) ||
                !dbus_message_iter_append_basic(&args, DBUS_TYPE_BYTE, &rpm_value)) {
                fprintf(stderr, "Out Of Memory!\n");
                exit(1);
            }

            // Send the message and flush the connection
            if (!dbus_connection_send(conn, msg, NULL)) { 
                fprintf(stderr, "Out Of Memory!\n"); 
                exit(1);
            }
            dbus_connection_flush(conn);

            // Free the message
            dbus_message_unref(msg);

        } else {
            pthread_mutex_unlock(&bufferMutex);
            usleep(1000);
        }
    }
    return NULL;
}

