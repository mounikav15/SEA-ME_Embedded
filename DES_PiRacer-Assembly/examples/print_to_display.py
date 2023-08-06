# Copyright (C) 2022 twyleg
import os
import pathlib
import time
import socket
import threading
import keyboard
from piracer.vehicles import PiRacerBase, PiRacerStandard

FILE_DIR = pathlib.Path(os.path.abspath(os.path.dirname(__file__)))


def get_ip_address():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        # doesn't have to be reachable, its just to get the iface IP
        s.connect(('10.255.255.255', 1))
        IP = s.getsockname()[0]
    except:
        IP = '127.0.0.1'
    finally:
        s.close()
    return IP


def print_battery_report(vehicle: PiRacerBase, stop_event: threading.Event):
    while not stop_event.is_set():
        battery_voltage = vehicle.get_battery_voltage()
        battery_current = vehicle.get_battery_current()
        power_consumption = vehicle.get_power_consumption()
        ip_address = get_ip_address()  # get ip address.

        display = vehicle.get_display()

        output_text = 'U={0:0>6.3f}V\nI={1:0>8.3f}mA\nP={2:0>6.3f}W\nIP={3}'.format(battery_voltage, battery_current,
                                                                                    power_consumption, ip_address)

        display.fill(0)
        display.text(output_text, 0, 0, 'white', font_name=FILE_DIR / 'fonts/font5x8.bin')
        display.show()
        time.sleep(0.5)


if __name__ == '__main__':
    piracer = PiRacerStandard()
    stop_event = threading.Event()

    # Start the display thread
    display_thread = threading.Thread(target=print_battery_report, args=(piracer, stop_event))
    display_thread.start()

    keyboard.add_hotkey('shift+g', lambda: stop_event.set())  # Stop the thread when 'Ctrl+P' is pressed.

    display_thread.join()  # Wait for the display thread to finish.

    # Add other scripts here
    # They will run while the display is being updated in its own thread
