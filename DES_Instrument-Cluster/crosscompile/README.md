# Cross-Compile between Qt5 and Raspberry Pi

## Environment 
- Laptop (Qt5): Ubuntu 20.04 / Raspberry Pi 4B: Raspbian Lite 64 bit
### If one of them is over Ubuntu 20, this proccess isn't working properly!!!!!!

## [Raspberry Pi]

### Add following line to /etc/apt/sources.list

```bash
deb-src http://raspbian.raspberrypi.org/raspbian/ buster main contrib non-free rpi
```

### Update the System following command

#### When Update firmware, you need to choose stable version. You can check the other version in commits. 
#### We choose 5.15.84 version.
##### [Rpi-firmware](https://github.com/raspberrypi/rpi-firmware)

```bash
sudo apt-get update
sudo apt-get dist-upgrade
sudo reboot
sudo rpi-update (firmware_version)
sudo reboot
```

##### If error occurs like "The following signatures couldn't be verified because the public key is not available: NO_PUBKEY (number)" -> Using command below

```bash
sudo -s
apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys (number)
apt update
exit
```

### Install some required packages 

```bash
sudo apt-get build-dep qt5-qmake
sudo apt-get build-dep libqt5gui5
sudo apt-get build-dep libqt5webengine-data
sudo apt-get build-dep libqt5webkit5
sudo apt-get install libudev-dev libinput-dev libts-dev libxcb-xinerama0-dev libxcb-xinerama0 gdbserver
```

### Create a Directory

```bash
sudo mkdir /usr/local/qt5.15
sudo chown -R pi:pi /usr/local/qt5.15
```

## [Ubuntu Host Machine]

### Update Ubuntu and install some required package

```bash
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install gcc git bison python gperf pkg-config gdb-multiarch
sudo apt install build-essential
```

### Create directory structure

```bash
sudo mkdir ~/Documents/Qt-CrossCompile-RaspberryPi
sudo mkdir ~/Documents/Qt-CrossCompile-RaspberryPi/raspberrypi4
sudo mkdir ~/Documents/Qt-CrossCompile-RaspberryPi/raspberrypi4/build
sudo mkdir ~/Documents/Qt-CrossCompile-RaspberryPi/raspberrypi4/sysroot
sudo mkdir ~/Documents/Qt-CrossCompile-RaspberryPi/raspberrypi4/sysroot/usr
sudo mkdir ~/Documents/Qt-CrossCompile-RaspberryPi/raspberrypi4/sysroot/opt
sudo chown -R 1000:1000 ~/Documents/Qt-CrossCompile-RaspberryPi/raspberrypi4
cd ~/Documents/Qt-CrossCompile-RaspberryPi/raspberrypi4
```

### Download Qt Resources and Modify mkspec to use our compiler

```bash
sudo wget http://download.qt.io/archive/qt/5.15/5.15.2/single/qt-everywhere-src-5.15.2.tar.xz
sudo tar xfv qt-everywhere-src-5.15.2.tar.xz

cp -R qt-everywhere-src-5.15.2/qtbase/mkspecs/linux-arm-gnueabi-g++ qt-everywhere-src-5.15.2/qtbase/mkspecs/linux-arm-gnueabihf-g++
sed -i -e 's/arm-linux-gnueabi-/arm-linux-gnueabihf-/g' qt-everywhere-src-5.15.2/qtbase/mkspecs/linux-arm-gnueabihf-g++/qmake.conf
```

### Download Cross-compiler

```bash
sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu
```

### Building Sysroot from Device

```bash
rsync -avzS --rsync-path="rsync" --delete <pi_username>@<pi_ip_address>:/lib/ sysroot/lib
rsync -avzS --rsync-path="rsync" --delete <pi_username>@<pi_ip_address>:/usr/include/ sysroot/usr/include
rsync -avzS --rsync-path="rsync" --delete <pi_username>@<pi_ip_address>:/usr/lib/ sysroot/usr/lib
rsync -avzS --rsync-path="rsync" --delete <pi_username>@<pi_ip_address>:/opt/vc/ sysroot/opt/vc

sudo apt install symlinks
symlinks -rc sysroot
```

### Compiling Qt

#### Before compiling, we use aarch64 for device, so we need to add it to our directory
##### [device - aarch64](https://code.qt.io/cgit/qt/qtbase.git/tree/mkspecs/devices/linux-rasp-pi4-aarch64)
#### 1. Make a directory named "linux-rasp-pi4-aarch64" in qt-everywhere-src-5.15.2/qtbase/mkspecs/devices/
#### 2. Download 2 files following upper url (qmake.conf, qplatformdefs.h) and add in "linux-rasp-pi4-aarch64" directory

#### If you finished previous process, following the command below

```bash
cd build
../qt-everywhere-src-5.15.2/configure -release -opengl es2  -eglfs -device linux-rasp-pi4-aarch64 -device-option CROSS_COMPILE=aarch64-linux-gnu- -sysroot ~/Documents/Qt-CrossCompile-RaspberryPi/raspberrypi4/sysroot -prefix /usr/local/qt5.15 -extprefix ~/Documents/Qt-CrossCompile-RaspberryPi/raspberrypi4/qt5.15 -opensource -confirm-license -skip qtscript -skip qtwayland -skip qtwebengine -nomake tests -make libs -pkg-config -no-use-gold-linker -v -recheck
```

##### If error occurs like "~ numeric_limits ~" 
##### -> Add 3 lines top of ~/Documents/Qt-CrossCompile-RaspberryPi/raspberrypi4/qt-everywhere-src-5.15.2/qtbase/src/corelib/global/qglobal.h

```bash
#ifdef __cplusplus
#include <limits>
#endif
```

#### Previous success without error, following command below

```bash
make -j4
make install
```

### Send qt5.15 to RaspberryPi

```bash
cd ~/Documents/Qt-CrossCompile-RaspberryPi/raspberrypi4
rsync -avzS --rsync-path="rsync" qt5.15/ --delete <pi_username>@<pi_ip_address>:/usr/local/qt5.15/
```

### reference: 
#### 1. [QT 5.15 CROSS COMPILE FOR RASPBERRY COMPUTE MODULE 4 ON UBUNTU 20 LTS](https://www.interelectronix.com/qt-515-cross-compilation-raspberry-compute-module-4-ubuntu-20-lts.html)
#### 2. [Cross-Compile Qt 6 for Raspberry Pi](https://wiki.qt.io/Cross-Compile_Qt_6_for_Raspberry_Pi)