Getting Started on the NRF51822
===============================

This guide explains how to install the tools necessary to develop for
the NRF51822.

Install the Toolchain
=====================

The NRF51822 run an ARM Cortex-M0, therefore the [gcc-arm-embedded](https://launchpad.net/gcc-arm-embedded/+download)
compiler will be used. Put the binaries for your platform in the bin directory.

On Linux you will also need 32 bit libs if you don't have them:

	sudo apt-get install lib32z1 lib32ncurses5 lib32bz2-1.0

Test that you have the toolchain by running:
	
	arm-none-eabi-gcc --version

Download the Nordic SDK
=========================

You will need the Nordic SDK which provides libraries for the device peripherals and
the softdevice.

You will also need the softdevice which is Nordic's BLE software stack. There
are multiple softdevicesL the S110 is for BLE peripherals, the S120 is for BLE
centrals, and the S130 supports both. Currently our examples are for the S110.

1. Download the Softdevice. We are currently using [S110v7.1.0](https://www.nordicsemi.com/eng/nordic/download_resource/30082/12/23994448)

2. Download and extract the SDK. Our tools currently support SDK 6 and 7. 
Our examples are written for [SDK 7.2](http://developer.nordicsemi.com/nRF51_SDK/nRF51_SDK_7.2.0_cf547b5.zip).


Get a programmer setup
======================

Currently we support programming over SWD using a [tag-connect pogo pin header](http://www.tag-connect.com/catalog/6),
and a [j-link](https://www.segger.com/jlink-debug-probes.html) programmer.
We will soon provide a uart bootloader, however a j-link is still necessary to flash
the bootloader.

You should Download and install the [Segger flashing tools](https://www.segger.com/jlink-software.html)
for your platform.


Setup one of the Example Programs
=================================

To flash one of the example programs you will have to edit the Makefile in 
the example application. Edit the Makefile as follows:

1. Uncomment and set the SDK_VERSION variable to match the SDK that you
downloaded (either 6 or 7)

2. Uncomment and set the SDK_PATH to point to the root of your extracted SDK

3. Uncomment and set the SOFTDEVICE variable to point to the softdevice
that you downloaded


Flash the example program
=========================
To flash the example program you will need the [nrf51-pure-gcc-setup](http://www.github.com/lab11/nrf51-pure-gcc-setup) 
repo to be located in the /software directory. This will happen for you if you cloned
the Nucleum repo recursively. If this repo is in the correct location then run:

	1. make flash-softdevice 

This programs the softdevice that you downloaded onto Nucleum

	2. make flash 
	
This flashs the example program onto Nucleum

For a complete set of commands including erasing and debugging, 
see the nrf51-pure-gcc-setup README.
