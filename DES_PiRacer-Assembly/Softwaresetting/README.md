## Install

Tested on the following Hardware:

* Raspberry Pi 4 Model B 4GB

Tested on the following distributions:      

* Ubuntu Server 22.04.1 LTS (64-Bit)


### Add additional sources

If you run **Ubuntu**, add the following sources first:

    sudo -s
    echo "deb http://archive.raspberrypi.org/debian/ buster main" >> /etc/apt/sources.list
    apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys 7FA3303E
    apt update
    exit

If you're facing messages like the following when trying to install packages
via apt:

    pi@ubuntu:/home/pi# sudo apt install PACKAGES...
    Waiting for cache lock: Could not get lock /var/lib/dpkg/lock-frontend. It is held by process 1807 (unattended-upgr)

Disable the unattended upgrade feature by running the following command to disable:

    sudo dpkg-reconfigure unattended-upgrades

### Install dependencies:

    sudo apt update
    sudo apt install \
        gcc \
        v4l-utils \
        i2c-tools \
        raspi-config \
        python3-dev \
        python3-setuptools \
        libopencv-dev

### Setup periphery 

#### Mount boot partition (Ubuntu only)

On Ubuntu it is necessary to explicitly mount the boot partition before 
enabling the i2c controller and camera:

    mount /dev/mmcblk0p1 /boot/

#### Enable i2c and camera

Use the **raspi-config** tool to enable the following peripherals:

* i2c: Interface Options > I2C
* Camera: Interface Options > Camera

Afterwards, reboot:
    
    sudo reboot

### Install piracer-py package

    cd ~
    git clone https://github.com/dongdongO/SEA-ME_Main.git
    cd DES_PiRacer-Assembly/

    pip install piracer-py

    cd examples

## Examples

The following examples will show the basic functionality of the piracer-py package.

We didn't use virtual environment because of the PATH issues.
