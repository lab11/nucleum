Getting Started on the nRF51822
===============================

This guide explains how to install the tools necessary to develop for
the nRF51822.

Install the Toolchain
---------------------

The NRF51822 run an ARM Cortex-M0, therefore the [gcc-arm-embedded](https://launchpad.net/gcc-arm-embedded/+download)
compiler will be used. Put the binaries for your platform in the bin directory.

On Linux you will also need 32 bit libs if you don't have them:

	sudo apt-get install lib32z1 lib32ncurses5 lib32bz2-1.0

Test that you have the toolchain by running:
	
	arm-none-eabi-gcc --version


Get a programmer setup
----------------------

Currently we support programming over SWD using a [tag-connect pogo pin header](http://www.tag-connect.com/catalog/6),
and a [j-link](https://www.segger.com/jlink-debug-probes.html) programmer.
We will soon provide a uart bootloader, however a j-link is still necessary to flash
the bootloader.

You should Download and install the [Segger flashing tools](https://www.segger.com/jlink-software.html)
for your platform.


Flash the example program
-------------------------
To flash the example program on to Nucleum:

    make flash 
	
This will automatically add the softdevice if it is not already
on the nRF51822.

For a complete set of commands including erasing and debugging, 
see the nrf51-pure-gcc-setup README.
