import dbus
import dbus.service
import dbus.mainloop.glib
from gi.repository import GLib

class ExampleService(dbus.service.Object):
    def __init__(self):
        # C : SERVER_BUS_NAME
        bus_name = dbus.service.BusName('in.softprayog.add_server', bus=dbus.SessionBus())
        # C : SERVER_OBJECT_PATH_NAME
        dbus.service.Object.__init__(self, bus_name, '/in/softprayog/adder')
        self.conn = dbus.SessionBus()

    # C : INTERFACE_NAME
    @dbus.service.method('in.softprayog.dbus_example')
    def send_battery_info(self, battery):
        # Create a new method call message
        # dbus.lowlevel.MethodCallMessage(SERVER_BUS_NAME, SERVER_OBJECT_PATH_NAME, INTERFACE_NAME, METHOD_NAME)
        msg = dbus.lowlevel.MethodCallMessage("in.softprayog.add_server", "/in/softprayog/adder", "in.softprayog.dbus_example", "add_numbers")
        
        # Append battery level as an argument to the message
        msg.append(battery)

        # Send the message and get the reply
        # reply가 서버쪽의 응답 메세지이므로 이에 대한 에러처리가 필요할 듯
        reply = self.conn.send_message_with_reply_and_block(msg, 2000)  # 2000 is a timeout in milliseconds

        return battery

if __name__ == '__main__':
    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)

    object = ExampleService()

    loop = GLib.MainLoop()
    print("Service Running...")
    while True:
        battery = input("Enter the remaining battery level: ")
        print(object.send_battery_info(battery))
