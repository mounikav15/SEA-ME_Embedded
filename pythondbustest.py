import dbus
import dbus.service
import dbus.mainloop.glib
from gi.repository import GLib
import time

class ExampleService(dbus.service.Object):
    def __init__(self):
        # Corresponds to SERVER_BUS_NAME in C code
        bus_name = dbus.service.BusName('org.team4.Des02', bus=dbus.SessionBus())
        # Corresponds to SERVER_OBJECT_PATH_NAME in C code
        dbus.service.Object.__init__(self, bus_name, '/CarInformation')
        self.conn = dbus.SessionBus()

    # Corresponds to INTERFACE_NAME in C code
    @dbus.service.method('org.team4.Des02.CarInformation')
    def send_battery_info(self, battery: dbus.Double) -> dbus.Double:
        # Create a new method call message
        # dbus.lowlevel.MethodCallMessage(SERVER_BUS_NAME, SERVER_OBJECT_PATH_NAME, INTERFACE_NAME, METHOD_NAME)
        msg = dbus.lowlevel.MethodCallMessage("org.team4.Des02", "/CarInformation", "org.team4.Des02.CarInformation", "setBattery")
        
        # Append battery level as an argument to the message
        msg.append(battery)

        # Send the message and get the reply
        # An error handling for the reply might be necessary
        reply = self.conn.send_message_with_reply_and_block(msg, 2000)  # 2000 is a timeout in milliseconds

        return battery

if __name__ == '__main__':
    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)

    object = ExampleService()

    loop = GLib.MainLoop()
    print("Service Running...")
    while True:
        battery = 80.0  # Convert input to float
        time.sleep(1)
        print(object.send_battery_info(battery))
