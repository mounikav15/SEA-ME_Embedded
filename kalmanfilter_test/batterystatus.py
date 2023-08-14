import dbus
import dbus.service
import dbus.mainloop.glib
from gi.repository import GLib
import time

from piracer.vehicles import PiRacerStandard

def get_battery_status(battery_voltage):
    # Equation for calculate leftover battery
    battery_status = (battery_voltage - 2.8 * 3.0) / (12.3 - 2.8 * 3.0) * 100.0

    return battery_status


class ExampleService(dbus.service.Object):
    def __init__(self):
        # Corresponds to SERVER_BUS_NAME in Qt
        bus_name = dbus.service.BusName('org.team4.Des02', bus=dbus.SessionBus())
        # Corresponds to SERVER_OBJECT_PATH_NAME in Qt
        dbus.service.Object.__init__(self, bus_name, '/CarInformation')
        self.conn = dbus.SessionBus()

    # Corresponds to INTERFACE_NAME in Qt
    @dbus.service.method('org.team4.Des02.CarInformation')
    def send_battery_info(self, battery: dbus.Double) -> dbus.Double:
        # Create a new method call message : Qt has method(function) name "setBattery"
        # dbus.lowlevel.MethodCallMessage(SERVER_BUS_NAME, SERVER_OBJECT_PATH_NAME, INTERFACE_NAME, METHOD_NAME)
        msg = dbus.lowlevel.MethodCallMessage("org.team4.Des02", "/CarInformation", "org.team4.Des02.CarInformation", "setBattery")
        
        # Append battery level as an argument to the message
        msg.append(battery)

        # Send the message and get the reply
        # An error handling for the reply might be necessary
        reply = self.conn.send_message_with_reply_and_block(msg, 2000)  # 2000 is a timeout in milliseconds

        return battery
    

    # Corresponds to INTERFACE_NAME in Qt
    @dbus.service.method('org.team4.Des02.CarInformation')
    def send_gear_info(self, gear: dbus.Byte) -> dbus.Byte:
        # Create a new method call message : Qt has method(function) name "setGear"
        # dbus.lowlevel.MethodCallMessage(SERVER_BUS_NAME, SERVER_OBJECT_PATH_NAME, INTERFACE_NAME, METHOD_NAME)
        msg = dbus.lowlevel.MethodCallMessage("org.team4.Des02", "/CarInformation", "org.team4.Des02.CarInformation", "setGear")
        
        # Append battery level as an argument to the message
        msg.append(gear)

        # Send the message and get the reply
        # An error handling for the reply might be necessary
        reply = self.conn.send_message_with_reply_and_block(msg, 2000)  # 2000 is a timeout in milliseconds

        return gear


if __name__ == '__main__':
    piracer = PiRacerStandard()  # Create an instance of the PiRacerStandard class

    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)  # Set the DBus main loop

    object = ExampleService()  # Create an instance of the ExampleService class

    loop = GLib.MainLoop()  # Start the GLib main loop
    print("Service Running...")

    while True:  # Infinite loop
        battery_voltage = piracer.get_battery_voltage()  # Get the battery voltage from the PiRacer

        # Calculate the battery status using the provided formula
        battery = get_battery_status(battery_voltage)
        # battery_current = piracer.get_battery_current()
        # gear_status = piracer.get_movement_status()
        # power_consumption = piracer.get_power_consumption()

        # Check the throttle status to determine the gear status
        if piracer.throttle_pwm_controller.channels[piracer.PWM_THROTTLE_CHANNEL_LEFT_MOTOR_IN1].duty_cycle > 0:
            gear_status = "D"  # Drive
        elif piracer.throttle_pwm_controller.channels[piracer.PWM_THROTTLE_CHANNEL_LEFT_MOTOR_IN2].duty_cycle > 0:
            gear_status = "R"  # Reverse
        else:
            gear_status = "N"  # Neutral
        
        gear_byte = dbus.Byte(ord(gear_status))  # Convert the gear status to a byte format

        time.sleep(0.5)  # Delay for 0.5 seconds

        # Send the battery and gear info to the DBus
        object.send_battery_info(battery)
        object.send_gear_info(gear_byte)

