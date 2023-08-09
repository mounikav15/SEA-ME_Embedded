
#include <stdio.h>
#include <stdlib.h>
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
        print_dbus_error ("Connecting D-Bus Error");

    if (!conn) 
        exit(1);

    while (1) {
        int speed_value = 10;

        while (1) {
            ret = dbus_bus_request_name (conn, CLIENT_BUS_NAME, 0, &dbus_error);  

            if (ret == DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) 
                break;

            if (ret == DBUS_REQUEST_NAME_REPLY_IN_QUEUE) {
                fprintf (stderr, "Waiting for the bus ... \\n");
                sleep (1);  
                continue;
            }
            if (dbus_error_is_set (&dbus_error))
                print_dbus_error ("CLIENT_BUS_NAME Fail");
        }

        DBusMessage *request;
        if ((request = dbus_message_new_method_call (SERVER_BUS_NAME, SERVER_OBJECT_PATH_NAME, 
                           INTERFACE_NAME, "setSpeed")) == NULL) {
            fprintf (stderr, "Error in dbus_message_new_method_call\\n");
            exit (1);
        }

        DBusMessageIter iter;
        dbus_message_iter_init_append (request, &iter);

        if (!dbus_message_iter_append_basic (&iter, DBUS_TYPE_DOUBLE, &speed_value)) {
            fprintf (stderr, "Error in dbus_message_iter_append_basic\\n");
            exit (1);
        }

        DBusPendingCall *pending_return;
        if (!dbus_connection_send_with_reply (conn, request, &pending_return, -1)) {
            fprintf (stderr, "Error in dbus_connection_send_with_reply\\n");
            exit (1);
        }

        if (pending_return == NULL) {
            fprintf (stderr, "pending return is NULL");
            exit (1);
        }

        dbus_connection_flush (conn);          
        dbus_message_unref (request);  

        dbus_pending_call_block (pending_return);  

        DBusMessage *reply;
        if ((reply = dbus_pending_call_steal_reply (pending_return)) == NULL) {
            fprintf (stderr, "Error in dbus_pending_call_steal_reply");
            exit (1);
        }

        dbus_pending_call_unref (pending_return);  

        char *s;
        if (dbus_message_get_args (reply, &dbus_error, DBUS_TYPE_STRING, &s, DBUS_TYPE_INVALID)) {
            printf ("%s\\n", s);
        }
        else {
             fprintf (stderr, "Did not get arguments in reply\\n");
             exit (1);
        }

        dbus_message_unref (reply);  

        if (dbus_bus_release_name (conn, CLIENT_BUS_NAME, &dbus_error) == -1) {
             fprintf (stderr, "Error in dbus_bus_release_name\\n");
             exit (1);
        }
    }

    return 0;
}

void print_dbus_error (char *str) 
{
    fprintf (stderr, "%s: %s\\n", str, dbus_error.message);
    dbus_error_free (&dbus_error);
}

