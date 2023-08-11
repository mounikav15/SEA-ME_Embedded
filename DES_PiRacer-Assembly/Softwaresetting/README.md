## Install

Tested on the following Hardware:

* Raspberry Pi 4 Model B 4GB

Tested on the following distributions:      

* Rasbian Lite(Server) 64-Bit : latest version but update raspi-firmware version 5.15.84

### Download Raspbian
##### [Rpi-Imager](https://www.raspberrypi.com/software/)

You can download Raspberry Pi Imager in link. You can choose raspbian version in imager. 

We choose Rasbian Lite 64-Bit. 

Among the parts to be careful of, the USB erase function in Imager cannot initialize the file system, so you need to download the OS from Imager after initializing the USB with a program that initializes the file system.

### Basic Wi-Fi setting

If you know the Wi-Fi ssid and access password, you can add the following to the "wpa_supplicant.conf" file. 
Change my_ssid to your wifi's ssid and my_password to your own Wi-Fi access password.
You can edit the "wpa_supplicant.conf" file with the command below

```bash
sudo nano /etc/wpa_supplicant/wpa_supplicant.conf
```

```bash
network={
	ssid="my_ssid"
	psk="my_password"
}
```
 
If the ssid of my wireless router is iptime123 and the connection password is abcd1111, the final wpa_supplicant.conf file will look like the following.
```bash
pi@raspberrypi:~ $ cat /etc/wpa_supplicant/wpa_supplicant.conf
ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1
country=GB

network={
	ssid="iptime123"
	psk="abcd1111"
}
```

After confirming that the file was saved normally, reboot with the command below.

```bash
sudo reboot
```
 
When the reboot is complete, check if the Wi-Fi connection is normal with the command below. If the IP is assigned to the wlan0 item, it is normally connected. (IP varies depending on your router settings.)

```bash
pi@raspberrypi:~ $ ifconfig
wlan0: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 192.168.1.6  netmask 255.255.255.0  broadcast 192.168.1.255
```


## Update the Raspberrypi Firmware

##### When Update firmware, you need to choose stable version
##### [Rpi-firmware](https://github.com/raspberrypi/rpi-firmware)

We choose 5.15.84. You can check the version in commits.

```bash
sudo apt-get update
sudo apt-get dist-upgrade
sudo reboot
sudo rpi-update (firmware_version)
sudo reboot
```


### Install dependencies:

    sudo apt update
    sudo apt install \
        gcc \
        v4l-utils \
        i2c-tools \
        raspi-config \
        python3-dev \
        python3-setuptools \
        libopencv-dev \
        python3-pip \
    sudo apt upgrade
    sudo reboot

### Setup periphery 


#### Enable i2c and camera

Use the **raspi-config** tool to enable the following peripherals:

* i2c: Interface Options > I2C
* Camera: Interface Options > Camera

Afterwards, reboot:
    
    sudo reboot

### Install piracer-py package

    cd ~
    git clone https://github.com/dongdongO/SEA-ME_Embedded.git
    cd DES_PiRacer-Assembly/examples

    pip install piracer-py


## Examples

The following examples will show the basic functionality of the piracer-py package.

We didn't use virtual environment because of the PATH issues.
