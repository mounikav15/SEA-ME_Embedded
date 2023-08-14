# D-Bus Tutorial
<br>  

# Intorduction

In this repository, we test D-Bus by providing clear examples of how to implement both a client and a server in two popular programming languages: C and Python. D-Bus is a powerful inter-process communication protocol that is widely used in Linux for software components to communicate with each other.
<br>  

# DBus Basic Concepts
D-Bus (Desktop Bus) is an inter-process communication (IPC) mechanism that allows multiple processes to communicate with one another in a seamless and efficient manner. Originally developed for desktop environments, it's now widely used in various software systems, especially in Linux-based platforms. Here are some foundational concepts to understand:

### D-Bus Interface:   
The interface is like a contract, defining the set of methods (API) an object provides. It's a crucial element in D-Bus communication, ensuring that both the sender and receiver have a common understanding of the message structure and content. Each method in an object is uniquely identified by its interface name.

### D-Bus Name:   
Every application that wants to communicate over D-Bus must have a unique name. This name serves as the application's address on the D-Bus, ensuring that messages are delivered to the correct recipient. Both well-known and unique names exist in the D-Bus system. Well-known names are usually static, human-readable strings, while unique names are dynamically assigned by the D-Bus daemon each time a connection is established.

### D-Bus Object Path:   
Objects are the entities that expose methods that can be invoked over D-Bus. Each object is uniquely identified by its object path, which is a string that looks much like a filesystem path. This path provides a hierarchical way to organize and locate objects within an application.

### D-Bus Message:   
Communication in D-Bus is done through messages. There are several types of messages, including method calls, method returns, signals, and errors. A method call message is used when a client wants to invoke a method on a server. Signals are broadcast messages that any interested client can listen to. Errors are sent in response to a method call to indicate that something went wrong.

### D-Bus Daemon:  
At the heart of D-Bus is the D-Bus daemon. This daemon routes messages between applications. There are typically two main instances of the D-Bus daemon running on a Linux system: the system bus (for system-wide services) and the session bus (for user-specific applications).
<br>  
<br>  

D-Bus provides several advantages, such as:  

* Decoupling: D-Bus decouples the client from the server. The client doesn't need to know any details about the server's implementation or where it's located.
* Security: D-Bus provides fine-grained security controls, allowing you to specify who can send or receive messages.
* Flexibility: D-Bus is language-agnostic, and bindings exist for several programming languages, making it versatile for various applications.
<br>  

Understanding these concepts is fundamental to effectively using D-Bus for inter-process communication.


# How to Use These Codes  
### C Implementation:  
* add-client.c: Contains the C implementation of a D-Bus client.  
* add-server.c: Contains the C implementation of a D-Bus server. 
* You can use Makefile instead.   

### Python Implementation:  
* pydbus-client.py: Contains the Python implementation of a D-Bus client using the dbus module.  
* pydbus-server.py: Contains the Python implementation of a D-Bus server using the dbus module.  

To run the code, ensure you have the required libraries and dependencies installed. For the C implementations, you may need the D-Bus library, and for the Python implementations, the dbus-python package.

# Reference
* [D-Bus Tutorial](https://www.softprayog.in/programming/d-bus-tutorial)
* [Python D-Bus Tutorial](https://dbus.freedesktop.org/doc/dbus-python/)