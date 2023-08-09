import dbus
import dbus.service
import dbus.mainloop.glib
import glib

class ExampleService(dbus.service.Object):
    def __init__(self):
        bus_name = dbus.service.BusName('in.softprayog.add_server', bus=dbus.SessionBus())
        dbus.service.Object.__init__(self, bus_name, '/in/softprayog/adder')

    @dbus.service.method('in.softprayog.dbus_example')
    def add_numbers(self, message):
        # Parse the message
        try:
            number1, number2 = map(int, message.split())
            return str(number1 + number2)
        except Exception as e:
            # return an error message
            return str(e)

if __name__ == '__main__':
    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)

    object = ExampleService()

    loop = glib.MainLoop()
    print("Service Running...")
    loop.run()
