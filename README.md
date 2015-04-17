Atum-le
====

Atum-le is still a work in progress.

Atum-le is a ½ inch² drop-in sensor node module designed to enable rapid
prototyping of Bluetooth Low Energy embedded devices. 
The core of Atum-le is the NRF51822
SoC with an Arm Cortex M0 and BLE radio stack. Atum-le also includes an
antenna, 512 bytes of non-volatile FRAM memory, an RTC, and a tri-color LED.

Atum-le is designed to have two different usage patterns. First, three sides
of the device have castellated headers allowing Atum-le to be directly
soldered onto a carrier PCB. Atum-le handles all of the computation and
communication while the carrier board supplies power and attaches
sensors or other peripherals. In the second option, Atum-le can function
as a complete sensor node. On the bottom are pads for a 3.3 V regulator
and a battery. If needed, a peripheral or sensor board can be stacked on
top using the headers as a bridge.


History
-------

Atum-le is directly derived from [Atum](http://www.github.com/lab11/atum). The
addition of
'-le' signifies the modification of the original Atum to a node that supports
Bluetooth Low Energy (BLE).
Visit the Atum page to read more about its origins.

Name
----

The name is taken from the [Atum](http://www.github.com/lab11/atum) project.
Atum-le is currently in search of a unique name.

Usage
-----

A usage guide for the NRF51822 will be added soon.

PCB
---

Current progress on the hardware can be found in hardware/atum-le.

