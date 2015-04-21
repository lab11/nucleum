Nucleum
====

Nucleum is still a work in progress.

Nucleum is a ½ inch² drop-in sensor node module designed to enable rapid
prototyping of Bluetooth Low Energy embedded devices. 
The core of Nucleum is the NRF51822
SoC with an Arm Cortex M0 and BLE radio stack. Nucleum also includes an
antenna, 512 bytes of non-volatile FRAM memory, an RTC, and a tri-color LED.

Nucleum is designed to have two different usage patterns. First, three sides
of the device have castellated headers allowing Nucleum to be directly
soldered onto a carrier PCB. Nucleum handles all of the computation and
communication while the carrier board supplies power and attaches
sensors or other peripherals. In the second option, Nucleum can function
as a complete sensor node. On the bottom are pads for a regulator
and a battery. If needed, a peripheral or sensor board can be stacked on
top using the headers as a bridge.


History
-------

Nucleum is directly derived from [Atum](http://www.github.com/lab11/atum). It
changes the radio protocol to BLE and adds an on-board programmer. Like Atum, 
it is intended to be a building block for sensor platforms that only includes
the features that a sensor board needs.
Visit the [Atum](http://www.github.com/lab11/atum) page to read more about its 
origins.

Name
----

Nucleum is the latin accusative form of 'Nucleus' which, like [Atum](http://www.github.com/lab11/atum),
represents a basic building block of matter. The 'LE' in the spelling of Nucleum 
indicates its use as a Bluetooth Low Energy prototyping board, and the 'UM' at
the end of Nucleum represents Nucleum's development at the University of Michigan.

Usage
-----

A usage guide for the NRF51822 will be added soon.

PCB
---

Current progress on the hardware can be found in hardware/nucleum

