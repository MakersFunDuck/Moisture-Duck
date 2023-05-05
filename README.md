# Moisture-Duck
ATtiny13A Resistive Soil Sensor for Duck Lovers


Moisture-Duck helps you keep track of your plants' soil moisture and alerts you if they need watering. It utilizes the resistance measurement technique to gauge soil moisture levels. Two probes, in combination with the substrate, constitute a variable resistor. Greater soil moisture translates to improved conductivity and decreased electrical resistance. With the addition of a 12k fixed resistor, the system creates a voltage divider. The microcontroller's ADC measures this voltage. The design is inspired by the SparkFun Soil Moisture Sensor.

Description:
------------
The Moisture Duck is a device that measures soil moisture and emits an acoustic and optical signal if the soil is too dry. The device is based on ATtiny13, which spends most of its time in sleep mode and wakes up every eight seconds to take measurements. It can also be activated by pressing the TEST button, and the correct threshold value can be set by turning the potentiometer. To calibrate the device, it should be placed in dry soil and the potentiometer should be adjusted until the alarm is triggered. The device is compiled using MicroCore and default settings should be left unchanged. Arduino core functions and libraries are not used, and the makefile can be used for compilation without Arduino IDE. The fuse settings are -U lfuse:w:0x3b:m -U hfuse:w:0xff:m.

Compiling and Uploading:
------------

You can program the ATtiny either before soldering using an SOP adapter, or after soldering using ICSP pins. The AVR Programmer Adapter can help with this.
If using the Arduino IDE

    Make sure you have installed MicroCore.
    Go to Tools -> Board -> MicroCore and select ATtiny13.
    Go to Tools and choose the following board options:
        Clock: 128 kHz internal osc.
        BOD: BOD disabled
        Timing: Micros disabled
    Connect your programmer to your PC and to the ATtiny.
    Go to Tools -> Programmer and select your ISP programmer (e.g. USBasp).
    Go to Tools -> Burn Bootloader to burn the fuses.
    Open main.ino and click Upload.

If using the precompiled hex-file

    Make sure you have installed avrdude.
    Connect your programmer to your PC and to the ATtiny.
    Open a terminal.
    Navigate to the folder with the hex-file.
    Execute the following command (if necessary replace "usbasp" with the programmer you use):

    avrdude -c usbasp -p t13 -U lfuse:w:0x3b:m -U hfuse:w:0xff:m -U flash:w:main.hex

If using the makefile (Linux/Mac)

    Make sure you have installed avr-gcc toolchain and avrdude.
    Connect your programmer to your PC and to the ATtiny.
    Open a terminal.
    Navigate to the folder with the makefile and sketch.
    Run PROGRMR=usbasp make install to compile, burn the fuses and upload the firmware (change PROGRMR accordingly).
