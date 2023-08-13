# **DES Project - CAN with Arduino**   


# Introduction

This document outlines the experiments conducted by our team, utilizing the Controller Area Network (CAN) communication protocol for data exchange between two Arduino boards. The CAN protocol is widely used in automobiles, robotics, embedded systems, and more, offering high reliability, efficiency, and flexibility, making it a standard in various application fields.

Our team has set a goal to leverage these capabilities of the CAN communication in applying it for communication between Raspberry Pi and Arduino. However, before implementing such communication protocol, we first tested whether CAN communication between Arduinos is successfully achieved.

This document details the process and results of such testing, so that you will gain a basic understanding of how to implement CAN communication between Arduinos and learn how to execute actual communication scenarios.  



# Background Information

Controller Area Network (CAN) is a robust and efficient communication protocol, widely used in various sectors requiring real-time data transmission between multiple nodes. Originating in automotive applications, CAN has expanded to areas like industrial automation, medical devices, and robotics.

CAN's efficiency stems from its design, which allows multiple devices on the same network to communicate without a host computer, improving data exchange. It ensures reliability through prioritized messages, error detection, and automatic re-transmission of faulty messages.

Moreover, CAN's scalability is due to its layered architecture, which allows for easy expansion or modification of the network. This document will provide a condensed understanding of the CAN protocol, focusing on its application in Arduino-based systems.  



# Project Goals and Objectives

The primary aim of our project is to maximize the capabilities of the Controller Area Network (CAN) communication protocol to enable effective communication between Arduino boards.

Here are the specific objectives of the project:

1. To understand the intricacies of the CAN protocol and how it facilitates real-time data transmission in various systems.
2. To conduct a successful pilot test of CAN communication between two Arduino boards, ensuring the efficient exchange of data between them.
3. To apply the knowledge and results obtained from the Arduino-Arduino CAN communication testing to implement effective communication between Arduino boards.
4. To ensure the Arduino-Arduino communication via CAN is efficient, reliable, and meets the needs of the desired applications.
5. To document all the processes, experiments, results, and conclusions comprehensively, providing a robust guide for future endeavors involving the CAN protocol.


By accomplishing these objectives, the project will not only verify the efficacy of CAN communication between Arduino boards but also provide a practical guide on how to implement efficient communication between Arduino boards using the CAN protocol.  


# Hardware Setting  

## Requirements

* 2 Arduino boards (either Uno or Nano)
* 1 Arduino CAN shield
* 1 MCP2515 module (you can also prepare 2 if you don't use the Arduino CAN shield)
* Several jumper wires
* Speed sensor
* 2 laptops  
  
## Connect

<img src="arduino_can_GPIO" alt="Alt text" width="50%" height="50%">


Please refer to the above image and the Arduino's GPIO map for connections.  


# Software Setting

If you want to test CAN on the configured Arduino, please follow these steps:

1. Install Arduino IDE on each laptop.
2. Download the GitHub repository at [https://github.com/autowp/arduino-mcp2515](https://github.com/autowp/arduino-mcp2515) as a zip file.
3. Upload the zipped folder to the Arduino IDE as a library on each laptop.
4. Connect Arduino to each laptop and execute the example provided in the repository.

For more detailed instructions, it's strongly recommended to read the README file of the [repository](https://github.com/autowp/arduino-mcp2515).

Additionally, if you want to communicate data from the speed sensor, please use the 'speedwrite' and 'speedread' codes.  



# Referencess

1. https://github.com/autowp/arduino-mcp2515.git
