# **Raspberrypi4 - Can Setting**



# Index
#### [Introduction](#introduction)
#### [Features](#features)
#### [Specification](#specification)
#### [Interfaces](#interfaces)
#### [Working with Raspberrypi](#working-with-raspberry-pi)
#### [Enable SPI interface](#enable-spi-interface)
#### [Install Libraries](#install-libraries)
#### [Testing](#testing)
#### [References](#references)
</br>


## Introduction
This is a 2-Channel CAN bus expansion HAT designed for Raspberry Pi, supports CAN2.0, features multi-onboard protection circuits, high anti-interference capability, and stable operation. It suits fields such as automotive devices or industrial automation.

## Features
- Standard Raspberry Pi 40PIN GPIO extension header, supports Raspberry Pi series boards.
- Adopts MCP2515 and SI65HVD230(or SN65HVD230) dual chips combined solution, allowing 2-channel CAN communication.
- Integrated power isolation, providing stable isolated voltage, requires no extra power supply for the isolated terminal.
- Onboard digital isolator, signal isolation makes communication safer, more stable, and better anti-interference.
- Onboard TVS (Transient Voltage Suppressor) diode, provides ESD protection and transient peak voltage protection.
- Onboard voltage translator, select 3.3V/5V operating voltage by jumper.
- Onboard 120Ω terminal resistor, configured by jumper.
- Breakout SPI control pins, for connecting with host control boards like STM32/Arduino.
- Comes with development resources and a manual.

## Specifications
- CAN controller: MCP2515
- CAN transceiver: SI65HVD230 (or SN65HVD230)
- Communication interface: SPI
- Operating voltage: 5V
- Logic level: 3.3V/5V
- Dimensions: 65.0 x 56.5 mm

## Interfaces

### CAN bus
| **PIN** | **Raspberry Pi (BCM)** | **Raspberry Pi (WPI)** | **Description** |
|---|---|---|---|
| 5V | 5V | 5V | 5V Power input |
| GND | GND | GND | Ground |
| MISO | 9 (MISO) | 13 (MISO) | SPI clock input |
| MOSI | 10 (MOSI) | 12 (MOSI) | SPI data input |
| SCK | 11 (SCLK) | 14(SCLK) | SPI data output |
| CS_0 | 8 (CE0) | 10 (CE0) | CAN_0 chip select |
| INT_0 | 23 (Default)/22 | 6 (Default)/5 | CAN_0 interrupt output |
| CS_1 | 7 (CE1) | 11 (CE1) | CAN_1 chip select |
| INT_1 | 25 (Default)/24 | 4 (Default)/3 | CAN_1 interrupt output |


### CAN bus
CAN module could process packets transmit/receive on the CAN bus. Packets transmit the first store packet to the related buffer and control register. 
Use the SPI interface to set the bits on the control register or enable transmit pin for transmitting. 

## Working with Raspberry Pi
The working voltage level of Raspberry Pi is 3.3V, therefore we need to set the VIO of 2-CH CAN HAT to 3.3V as below.

**Note:** When connecting to the Raspberry Pi 2b, 3b, and 4b boards, please connect the booster and nylon post to fix it to avoid the back of the CAN terminal touching the HDMI interface causing a short circuit, and avoid wrong connection or poor contact.

## Enable SPI interface
Open the terminal, and use the command to enter the configuration page.

```bash
sudo raspi-config
```

Choose Interfacing Options -> SPI -> Yes to enable the SPI interface.


<img src="https://github.com/dongdongO/SEA-ME_Main/assets/97011426/4bacc2cc-658a-41f0-80e4-9192e23d2188" width="60%" height="60%"/>


Reboot Raspberry Pi:

```bash
sudo reboot
```

Please make sure that the SPI interface was not used by other devices, you can check in the /boot/firmware/config.txt.

## Install libraries
* bcm2835

Open the terminal and run the commands below to install the bcm2835 library:

```bash
wget http://www.airspayce.com/mikem/bcm2835/bcm2835-1.60.tar.gz
tar zxvf bcm2835-1.60.tar.gz 
cd bcm2835-1.60/
sudo ./configure
sudo make
sudo make check
sudo make install
# For More：http://www.airspayce.com/mikem/bcm2835/
```

* wiringPi

```bash
#Open the Raspberry Pi terminal and run the following command
cd
sudo apt-get install wiringpi
#For Raspberry Pi systems after May 2019 (earlier than that can be executed without), an upgrade may be required:
wget https://project-downloads.drogon.net/wiringpi-latest.deb
sudo dpkg -i wiringpi-latest.deb
gpio -v
# Run gpio -v and version 2.52 will appear, if it doesn't it means there was an installation error

# Bullseye branch system using the following command:
git clone https://github.com/WiringPi/WiringPi
cd WiringPi
. /build
sudo gpio -v
# Run gpio -v and version 2.70 will appear, if it doesn't it means there was an installation error
```

* Python

Install python library:

```bash
#python3
sudo apt-get update
sudo apt-get install python3-pip
sudo apt-get install python3-pil
sudo apt-get install python3-numpy
sudo pip3 install RPi.GPIO
sudo pip3 install spidev 
sudo pip3 install python-can
```
### Preparation
* Insert the module into Raspberry Pi, and then modify the config.txt file:

```bash
sudo vi /boot/firmware/config.txt
```

* Append these statements to the file:

```bash
dtparam=spi=on
dtoverlay=mcp2515-can1,oscillator=16000000,interrupt=25
dtoverlay=mcp2515-can0,oscillator=16000000,interrupt=23
dtoverlay=spi-bcm2835-overlay
```

* Save and exit, then restart your Pi:
```bash
sudo reboot
```

* After restart, check if initialize successfully:
```bash
sudo dmesg | grep spi
```

<img src="https://github.com/dongdongO/SEA-ME_Main/assets/97011426/4464ea8e-a876-419a-add4-bee7b9cffb24" width="60%" height="60%"/>


* Set up CAN:
```bash
sudo ip link set can0 up type can bitrate 1000000
sudo ip link set can1 up type can bitrate 1000000
sudo ifconfig can0 txqueuelen 65536
sudo ifconfig can1 txqueuelen 65536
```

* For more information about CAN kernel instructions, please check:
```bash
# https://www.kernel.org/doc/Documentation/networking/can.txt
```

* Run "ifconfig":
```bash
ifconfig
```
<img src="https://github.com/dongdongO/SEA-ME_Main/assets/97011426/9cf69ea4-1313-4110-b758-5800dd810f18" width="80%" height="80%"/>


## Testing
If there is one 2-CH CAN HAT on hand, you can connect CAN0_H and CAN1_H and CAN0_L and CAN1_L of the module:

* install can-utils:
```bash
sudo apt-get install can-utils
```

* Open two terminal windows:
One terminal input receives the CAN0 data command:
```bash
candump can0
```

Another terminal input sends the CAN1 data command:
```bash
cansend can1 000#11.22.33.44
```

<img src="https://github.com/dongdongO/SEA-ME_Main/assets/97011426/997aa45f-9d24-4089-9acf-afed1bb234c2" width="80%" height="80%"/>



If you have two 2-CH CAN HAT on hand, you can directly connect CAN_H and CAN_L two by two. The effect is the same as the above, pay attention to match the communication rate, identify the ID, and output the interface serial number.

## References

- https://www.waveshare.com/wiki/2-CH_CAN_HAT
