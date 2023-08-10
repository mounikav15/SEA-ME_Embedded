
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h> 
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <dbus/dbus.h>

const char *const INTERFACE_NAME = "org.team4.Des02.CarInformation";
const char *const SERVER_BUS_NAME = "org.team4.Des02";
const char *const CLIENT_BUS_NAME = "org.team4.Des02.Client";
const char *const SERVER_OBJECT_PATH_NAME = "/CarInformation";
const char *const CLIENT_OBJECT_PATH_NAME = "/CarInformationClient";

DBusError dbus_error;  
void print_dbus_error (char *str);

int main (int argc, char **argv)
{
    DBusConnection *conn;  
    int ret;  

    dbus_error_init (&dbus_error);  

    conn = dbus_bus_get (DBUS_BUS_SESSION, &dbus_error);  

    if (dbus_error_is_set (&dbus_error))  
        print_dbus_error ("Connecting D-Bus Error\n");

    if (!conn) 
        exit(1);

    while (1) 
    {
        int16_t speed_value = 10.0;
        int16_t rpm_value = 10.0;

        while (1) 
        {
            ret = dbus_bus_request_name (conn, CLIENT_BUS_NAME, 0, &dbus_error);  

            if (ret == DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) 
                break;

            if (ret == DBUS_REQUEST_NAME_REPLY_IN_QUEUE) 
            {
                fprintf (stderr, "Waiting for the bus ... \n");
                sleep (1);  
                continue;
            }

            if (dbus_error_is_set (&dbus_error))
                print_dbus_error ("CLIENT_BUS_NAME Fail");
        }

        DBusMessage *request_speed;
        if ((request_speed = dbus_message_new_method_call(SERVER_BUS_NAME, SERVER_OBJECT_PATH_NAME, 
                            INTERFACE_NAME, "setSpeed")) == NULL) 
        {
            fprintf(stderr, "Error in dbus_message_new_method_call for setSpeed\n");
            exit(1);
        }

        DBusMessageIter iter_speed;
        dbus_message_iter_init_append(request_speed, &iter_speed);

        if (!dbus_message_iter_append_basic(&iter_speed, DBUS_TYPE_INT16, &speed_value)) 
        {
            fprintf(stderr, "Error in dbus_message_iter_append_basic for speed_value\n");
            exit(1);
        }

        DBusPendingCall *pending_return_speed;
        if (!dbus_connection_send_with_reply(conn, request_speed, &pending_return_speed, -1)) 
        {
            fprintf(stderr, "Error in dbus_connection_send_with_reply for setSpeed\n");
            exit(1);
        }

        if (pending_return_speed == NULL) 
        {
            fprintf(stderr, "pending return for setSpeed is NULL\n");
            exit(1);
        }

        dbus_connection_flush(conn);
        dbus_message_unref(request_speed);
        dbus_pending_call_block(pending_return_speed);

        DBusMessage *reply_speed;
        if ((reply_speed = dbus_pending_call_steal_reply(pending_return_speed)) == NULL) 
        {
            fprintf(stderr, "Error in dbus_pending_call_steal_reply for setSpeed\n");
            exit(1);
        }

        dbus_pending_call_unref(pending_return_speed);  

        
        char *reply_msg_speed;
        if (dbus_message_get_args(reply_speed, &dbus_error, DBUS_TYPE_STRING, &reply_msg_speed, DBUS_TYPE_INVALID)) 
        {
            printf("setSpeed Reply: %s\n", reply_msg_speed);
        } 
        else 
        {
            fprintf(stderr, "Did not get arguments in reply for setSpeed\n");
            exit(1);
        }
        

        dbus_message_unref(reply_speed);  

        /****************************************************************************************/

        DBusMessage *request_rpm;
        if ((request_rpm = dbus_message_new_method_call(SERVER_BUS_NAME, SERVER_OBJECT_PATH_NAME, 
                        INTERFACE_NAME, "setRpm")) == NULL) 
        {
            fprintf(stderr, "Error in dbus_message_new_method_call for setRPM\n");
            exit(1);
        }

        DBusMessageIter iter_rpm;
        dbus_message_iter_init_append(request_rpm, &iter_rpm);

        if (!dbus_message_iter_append_basic(&iter_rpm, DBUS_TYPE_INT16, &rpm_value)) 
        {
            fprintf(stderr, "Error in dbus_message_iter_append_basic for rpm_value\n");
            exit(1);
        }

        DBusPendingCall *pending_return_rpm;
        if (!dbus_connection_send_with_reply(conn, request_rpm, &pending_return_rpm, -1)) 
        {
            fprintf(stderr, "Error in dbus_connection_send_with_reply for setRPM\n");
            exit(1);
        }

        if (pending_return_rpm == NULL) 
        {
            fprintf(stderr, "pending return for setRPM is NULL\n");
            exit(1);
        }

        dbus_connection_flush(conn);
        dbus_message_unref(request_rpm);
        dbus_pending_call_block(pending_return_rpm);

        DBusMessage *reply_rpm;
        if ((reply_rpm = dbus_pending_call_steal_reply(pending_return_rpm)) == NULL) 
        {
            fprintf(stderr, "Error in dbus_pending_call_steal_reply for setRPM\n");
            exit(1);
        }

        dbus_pending_call_unref(pending_return_rpm);  

        
        char *reply_msg_rpm;
        if (dbus_message_get_args(reply_rpm, &dbus_error, DBUS_TYPE_STRING, &reply_msg_rpm, DBUS_TYPE_INVALID)) 
        {
            printf("setRPM Reply: %s\n", reply_msg_rpm);
        } 
        else 
        {
            fprintf(stderr, "Did not get arguments in reply for setRPM\n");
            exit(1);
        }
        

        dbus_message_unref(reply_rpm);  

        if (dbus_bus_release_name (conn, CLIENT_BUS_NAME, &dbus_error) == -1) 
        {
             fprintf (stderr, "Error in dbus_bus_release_name\\n");
             exit (1);
        }

        sleep(1);
    }

    return 0;
}

void print_dbus_error (char *str) 
{
    fprintf (stderr, "%s: %s\\n", str, dbus_error.message);
    dbus_error_free (&dbus_error);
}





