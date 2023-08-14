# DBus with Qt  
<br>  

# Introduction
This project aims to manage key vehicle information (speed, RPM, gear, battery status) and communicate it to a Qt program using D-Bus. The logic for transmitting and receiving information is implemented in C, Python, and Qt.
<br>  

# Project Goals
The primary goal of this repository is to understand the connection method of dbus and how dbus is used in various languages and frameworks before the final project.  
Specific objectives include:  

* Managing and providing vehicle information in real-time with precision.
* Accessing vehicle information in various languages and environments using D-Bus communication.
* Ensuring ease of maintenance by modularizing the code and enhancing scalability.
<br>  

# Details
### Sender  
A folder containing codes that send values to DBus.  
Four pieces of information (speed, RPM, battery status, gear status) are input into DBus.    

#### batterystatus.py:

Written in Python, this code calculates and checks the vehicle's battery status. It also discerns the state of forward and reverse motions to relay gear information. Ultimately, it updates this information on Qt at 1-second intervals using D-Bus.

### speedstatus.c:

Written in C, this code manages the vehicle's speed and RPM information. Currently, the value is fixed at 10, but there are plans to relay data received from CAN in the future. It uses D-Bus to pass this information to the D-Bus service implemented in Qt.

### QtReceiver  
Responsible for receiving the information sent from the sender folder and displaying it on the terminal. In the future, this received data is planned to be displayed on a QML-styled dashboard.

#### carinformation.cpp/h:

This C++ class manages key vehicle information (speed, RPM, gear, battery status) in an object-oriented manner. It provides a D-Bus interface to query or set this information from the outside.

#### server_register.pro:

This is the Qt project configuration file. It contains settings related to D-Bus and other project configurations.

#### test.xml:

This is the D-Bus interface definition file, containing information about available methods and interfaces. It is not manually edited but is written using the qdbuscpp2xml command.

# Usage
The chosen Qt compiler version is 5.15.0, and the Qt Creator version is 4.15.0.  
First, build and run the Qt program.  
Then, execute the codes in the sender folder as well.  
To start the battery status information D-Bus service with the Python script, use the following command:  
```bash
python3 batterystatus.py
```
<br>  

To compile and execute the C code, relaying speed and RPM information to the Qt D-Bus service, use:  
```bash
make
```
<br>  

# Reference
- [Q-Dbus](https://doc.qt.io/qt-5/qtdbus-index.html)
- [qdbuscpp2xml](https://doc.qt.io/qt-6/qdbusxml2cpp.html)