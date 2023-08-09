#include <stdio.h>
#include <stdlib.h>
#include <dbus/dbus.h>

const char *const SERVER_BUS_NAME = "org.team4.Des02";
const char *const OBJECT_PATH = "/CarInformation";
const char *const INTERFACE_NAME = "org.team4.Des02.CarInformation";

void print_dbus_error(char *msg, DBusError *err) {
    fprintf(stderr, "%s: %s\n", msg, err->message);
    dbus_error_free(err);
}

int main() {
    DBusConnection *conn;
    DBusError err;
    DBusMessage *msg;
    DBusPendingCall *pending;
    double speed_value = 10.0;

    // Initialize error value
    dbus_error_init(&err);

    // Connect to D-Bus
    conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if (dbus_error_is_set(&err)) {
        print_dbus_error("Connection Error", &err);
        return EXIT_FAILURE;
    }
    if (!conn) {
        fprintf(stderr, "Connection Null\n");
        return EXIT_FAILURE;
    }

    // Create a new method call message
    msg = dbus_message_new_method_call(SERVER_BUS_NAME, OBJECT_PATH, INTERFACE_NAME, "setSpeed");
    if (!msg) {
        fprintf(stderr, "Message Null\n");
        return EXIT_FAILURE;
    }

    // Append arguments to the message
    if (!dbus_message_append_args(msg, DBUS_TYPE_DOUBLE, &speed_value, DBUS_TYPE_INVALID)) {
        fprintf(stderr, "Out of Memory!\n");
        return EXIT_FAILURE;
    }

    // Send message and get a handle for a reply
    if (!dbus_connection_send_with_reply(conn, msg, &pending, -1)) {
        fprintf(stderr, "Out of Memory!\n");
        return EXIT_FAILURE;
    }
    if (!pending) {
        fprintf(stderr, "Pending Call Null\n");
        return EXIT_FAILURE;
    }
    dbus_connection_flush(conn);

    // Free message
    dbus_message_unref(msg);

    // Block until we receive a reply
    dbus_pending_call_block(pending);

    // Get the reply message
    msg = dbus_pending_call_steal_reply(pending);
    if (!msg) {
        fprintf(stderr, "Reply Null\n");
        return EXIT_FAILURE;
    }

    // Print reply (assuming it's a string)
    char *reply_string;
    if (dbus_message_get_args(msg, &err, DBUS_TYPE_STRING, &reply_string, DBUS_TYPE_INVALID)) {
        printf("Received reply: %s\n", reply_string);
    } else {
        print_dbus_error("Failed to get reply", &err);
    }

    // Free the pending message handle
    dbus_pending_call_unref(pending);

    // Cleanup
    dbus_message_unref(msg);
    dbus_connection_unref(conn);

    return EXIT_SUCCESS;
}
