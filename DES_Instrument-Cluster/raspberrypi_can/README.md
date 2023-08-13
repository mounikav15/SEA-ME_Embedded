# **Raspberrypi4 - Arduino CAN**

# Result

<img src="Images/final_result" alt="Alt text" width="50%" height="50%"/>


# Introduction
This repository provides comprehensive guidance on how to establish CAN (Controller Area Network) communication between an Arduino and a Raspberry Pi 4. With the help of this guide, enthusiasts and developers can easily set up and configure their hardware for successful data exchange via the CAN protocol.  

# Background Information
CAN, or Controller Area Network, is a robust communication protocol commonly used in automotive and industrial applications due to its resilience and efficiency. It allows multiple devices to communicate without the need for a host computer. This project explores the interfacing of Raspberry Pi 4, a popular single-board computer, and Arduino, a widely-used microcontroller platform, using the CAN protocol.  



# Project Goals and Objectives

This project assumes the completion and availability of the arduino_can repository. Building upon that foundation, the objectives for this repository include:  

1. Understanding CAN Protocol: Grasp the fundamentals and importance of the CAN communication protocol.
2. Hardware Setup: Guide users in configuring the Arduino and Raspberry Pi 4 for CAN communication.
3. Data Exchange: Successfully transmit and receive speed data between the two devices.
4. Documentation: Provide a thorough walkthrough and reference material for users of all experience levels.


# Hardware Setting

## Requirements
* Arduino Uno: Any Arduino board with CAN capability or with a CAN shield/module.
* Raspberry Pi 4: Ensure it's updated to the latest firmware and OS.
* CAN Interface Module/Shield: For Arduino if it doesn't natively support CAN.
* CAN Hat or Module for Raspberry Pi 4: Used to enable CAN capability on the Pi.
* Jumper Wires: For connections.
* Speed Sensor : 
* Power Supply: Appropriate for both Raspberry Pi and Arduino.
* (Optional) Breadboard: For easy prototyping.  


## Connect

<img src="Images/arduino_rasp" alt="Alt text" width="50%" height="50%"/>  
1. Power Up: Ensure both the Arduino and Raspberry Pi 4 are powered off. Connect their power supplies.
2. Set Up CAN Interfaces: Attach the CAN module/shield to the Arduino and the CAN hat/module to the Raspberry Pi 4.
3. Connections: Using jumper wires, connect the CAN High and CAN Low pins between the Arduino and Raspberry Pi. It's crucial to ensure that these connections match.
4. Grounding: Connect the ground (GND) of the Arduino to the ground (GND) of the Raspberry Pi 4 to establish a common ground.
5. Power On: Once all connections are secured, power on both devices. Ensure that there's no short circuit or incorrect connections.  


Note: Before running any software, always double-check your connections to prevent any damage to your devices.

# Software Setting

Follow the software setting readme.


# Reference

- [RaspberryPi CAN](https://forums.raspberrypi.com/viewtopic.php?t=296117)

