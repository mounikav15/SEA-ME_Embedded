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

            if (recvbytes)
            {
                int disk_rpm = (frame.data[0] << 8) + frame.data[1];
                *rpm = (uint8_t)((float)disk_rpm / 2.6);
                *speed = (uint8_t)(*rpm * 3.4);
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

        // printf("CAN Data - Speed: %d RPM: %d\n", speed_value, rpm_value);

        pthread_mutex_lock(&bufferMutex);

        buffer[bufferIndex].speed = speed_value;
        buffer[bufferIndex].rpm = rpm_value;
        bufferIndex = (bufferIndex + 1)%BUFFER_SIZE;

        pthread_mutex_unlock(&bufferMutex);
    }
    return NULL;
}

void *dbusSendThread(void *arg) {
    DBusConnection *conn = (DBusConnection *)arg;
    DBusMessage *speed_msg, *rpm_msg;
    DBusMessageIter speed_args, rpm_args;
    DBusPendingCall *speed_pending, *rpm_pending;

    while (1) 
    {
        pthread_mutex_lock(&bufferMutex);

        int currentIndex = bufferIndex - 1;

        if (currentIndex < 0)
        {
            currentIndex = BUFFER_SIZE - 1;
        }

        uint8_t speed_value = buffer[currentIndex].speed;
        uint8_t rpm_value = buffer[currentIndex].rpm;


        pthread_mutex_unlock(&bufferMutex);

        printf("CAN Data - Speed: %d RPM: %d\n", speed_value, rpm_value);

        // Send speed_value
        speed_msg = dbus_message_new_method_call(SERVER_BUS_NAME, SERVER_OBJECT_PATH_NAME, INTERFACE_NAME, "setSpeed");
        if (NULL == speed_msg) { 
            fprintf(stderr, "Speed Message Null\n");
            exit(1);
        }
        dbus_message_iter_init_append(speed_msg, &speed_args);
        if (!dbus_message_iter_append_basic(&speed_args, DBUS_TYPE_BYTE, &speed_value)) {
            fprintf(stderr, "Out Of Memory for setSpeed!\n");
            exit(1);
        }

        if (!dbus_connection_send_with_reply(conn, speed_msg, &speed_pending, -1)) { 
            fprintf(stderr, "Out Of Memory for setSpeed!\n"); 
            exit(1);
        }
        if (NULL == speed_pending) { 
            fprintf(stderr, "Speed Pending Call Null\n"); 
            exit(1);
        }
        dbus_connection_flush(conn);
        dbus_message_unref(speed_msg);

        dbus_pending_call_block(speed_pending);
        speed_msg = dbus_pending_call_steal_reply(speed_pending);
        if (NULL == speed_msg) {
            fprintf(stderr, "Speed Reply Null\n");
            exit(1);
        }
        char *speed_reply_msg;
        if (dbus_message_get_args(speed_msg, &dbus_error, DBUS_TYPE_STRING, &speed_reply_msg, DBUS_TYPE_INVALID)) {
            printf("setSpeed Reply: %s\n", speed_reply_msg);
        }
        dbus_message_unref(speed_msg);
        dbus_pending_call_unref(speed_pending);

        // Send rpm_value
        rpm_msg = dbus_message_new_method_call(SERVER_BUS_NAME, SERVER_OBJECT_PATH_NAME, INTERFACE_NAME, "setRpm");
        if (NULL == rpm_msg) { 
            fprintf(stderr, "RPM Message Null\n");
            exit(1);
        }
        dbus_message_iter_init_append(rpm_msg, &rpm_args);
        if (!dbus_message_iter_append_basic(&rpm_args, DBUS_TYPE_BYTE, &rpm_value)) {
            fprintf(stderr, "Out Of Memory for setRpm!\n");
            exit(1);
        }

        if (!dbus_connection_send_with_reply(conn, rpm_msg, &rpm_pending, -1)) { 
            fprintf(stderr, "Out Of Memory for setRpm!\n"); 
            exit(1);
        }
        if (NULL == rpm_pending) { 
            fprintf(stderr, "RPM Pending Call Null\n"); 
            exit(1);
        }
        dbus_connection_flush(conn);
        dbus_message_unref(rpm_msg);

        dbus_pending_call_block(rpm_pending);
        rpm_msg = dbus_pending_call_steal_reply(rpm_pending);
        if (NULL == rpm_msg) {
            fprintf(stderr, "RPM Reply Null\n");
            exit(1);
        }
        char *rpm_reply_msg;
        if (dbus_message_get_args(rpm_msg, &dbus_error, DBUS_TYPE_STRING, &rpm_reply_msg, DBUS_TYPE_INVALID)) {
            printf("setRpm Reply: %s\n", rpm_reply_msg);
        }
        dbus_message_unref(rpm_msg);
        dbus_pending_call_unref(rpm_pending);

        usleep(500000);

    }

    return NULL;
}