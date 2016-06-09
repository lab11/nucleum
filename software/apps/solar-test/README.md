This folder stores the firmware to test and store data recorded by the pressure sensor
on the Monoxalyze board. This test is intended to periodically record and
store pressure and temperature data as a test for pressure sensitive wakeup.
The rest of the guide explains how to build and program the NRF51822 running 
on Monoxalyze.

Setup
=====

1. Make sure you have the [arm-none-eabi-gcc](https://launchpad.net/gcc-arm-embedded)
toolchain. You just need the binaries for your platform.

2. Get the nRF51822 SDK and S110 soft device from the
[downloads page](https://www.nordicsemi.com/eng/Products/Bluetooth-Smart-Bluetooth-low-energy/nRF51822?resource=20339).
You want the "nRF51 SDK Zip File" and the "S110 nRF51822 SoftDevice (Production ready)".
You do need to buy a nRF51822 evm kit to get access to these, because companies
are the worst. Edit the SOFTDEVICE variable in the makefile to point to the 
hex file of the softdevice you are using (currently using s110-7.1.0.hex).

3. Get the [Segger flashing tools](http://www.segger.com/jlink-software.html)
for your platform.

4. Get the [nrf51-pure-gcc-setup](http://github.com/lab11/nrf51-pure-gcc-setup).


Install the Application
======================

1. Make sure the path to the SDK is set correctly in the application
makefile (or override it in your environment).

1. Make sure your SDK version is set correctly in the application makefile
(currently support version 6 and version 7)

1. Make sure the path to the TEMPLATE (nrf51-pure-gcc-setup) is set 
correctly in the application makefile. 

1. Now compile and load the application code.

        make flash

Run the Test
============

1. After reset the application will collect 8 hours of pressure and temperature 
Data. The data will start at flash 10k. 

Retrieve the Data
================

1. After the devices has been collecting data for 8 hours run make collect
with the device connected to the JLink to pull the data from the chip,
parse the data into a readable csv, and save it in a data folder


