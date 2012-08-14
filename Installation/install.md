# Installation of the FCBInfinity #

<p color="red">[WORK IN PROGRESS] This document is still in draft, I'll be adding much more information over the next couple of days!</p>

Please note, the installation of the FCBInfinity is NOT a simple plug and play modification. You'll need to drill, solder, paint, etc. so it's not for the faint of heart. However once you complete the installation you'll end up with one of the most versatile pedalboards. Since the programming is open source you can program the unit to control any device you want via MIDI. I still only recommend you perform this modification yourself if you're handy with tools. You're looking for an easier modification I'd recommend the UnO or Eureka chip, which is a simple plug and play modification.

## Prerequisites ##
Before you start you'll need the following:  
- The FCBInfinity board  
- All the components on the [parts list](https://github.com/mackatack/FCBInfinity/blob/master/Partlist.md)  
- Tools and materials; soldering iron, soldering tin, clippers, screwdrivers, new wires, shrinktubing, etc.  
- A working [Arduino SDK (software development kit) installation with the Teensyduino addition](http://www.pjrc.com/teensy/td_download.html)    
- An USB-A to Micro USB cable to connect the Teensy board to your computer  
- The [FCBInfinity source code](https://github.com/mackatack/FCBInfinity/tree/master/Teensyduino/fcbinfinity)  


## Step 1.1: Soldering the new PCB ##
Lets **not** start with gutting your old Pedal board, but first lets focus on getting the new stuff to work. You'll need the components in the [parts list](https://github.com/mackatack/FCBInfinity/blob/master/Partlist.md), the FCBInfinity board, soldering tools and wires. We're going to test if everything works using USB-power through the Teensy board that we'll solder onto the FCBInfinity board.  

Take the FCBInfinity PCB and plug in the components in the right locations. You'll end up with a board as seen in the pictures below. Please notice that the Max7219 chip and the three Led-Digits need to be on the bottom side. Please make sure that the components are in the correct position, desoldering a component that's upside-down is no fun.  
There's no need to push the Teensy board all the way into the board, in fact you might make a short somewhere if you do. Just push in the Teensy just far enough so you can solder it properly at the bottom side of the board.

**Important**: The FCBInfinity v3 board gives room to add 4 extra buttons or leds beside the regular push buttons. If you need more than four, you can use the 8 pins that are located on the inside of the Teensy, marked PA0 to PA7 (see the last picture below). I recommend you solder [angled header pins](http://www.conrad.nl/medias/global/ce/7000_7999/7300/7340/7341/734133_BB_00_FB.EPS_250.jpg) ([link](http://www.conrad.nl/ce/nl/product/734099/Male-connector-RM-254-haaks-Rastermaat-254-mm-Aantal-polen-2-x-4-Nominale-stroom-3-A-STL2-1470AGT-008U-MPE-Garry-1/0224620&ref=list)) to these 8 holes BEFORE soldering the Teensy onto the FCBInfinity board! These header pins are very cheap, so if you are able to find them I'd recommend you solder them on, so you can add upto 12 additional buttons or LEDs.
  
[<img src="resources/20120726_223457.jpg?raw=true" height=100/>](resources/20120726_223457.jpg)
[<img src="resources/20120726_224826.jpg?raw=true" height=100/>](resources/20120726_224826.jpg)
[<img src="resources/20120726_224902.jpg?raw=true" height=100/>](resources/20120726_224902.jpg)

## Step 1.2: Testing your board##
Once everything is soldered onto the new circuit board, we need to test it. Lets first start by connecting the USB cable to your computer and the Teensy board. The Led Digits might light up, then again they might not. In any case there should be a blinking led on the Teensy board. That's the default program that comes with a fresh new Teensy. If that works we can upload the the FCBInfinity software.  

Fire up the Arduino SDK that you've installed earlier and open the FCBInfinity.ino file. Before you do anything we need to setup the Arduino toolkit a little bit: under [Tools] set the board type to "Teensy++ 2.0" and set USB-type to "Serial".

Now you can just press the Upload button in the toolbar. The Arduino SDK will compile the code and if everything went ok the Teensy should reboot and the Led digits should flash "Inf." and then display '000'. At this time the FCBInfinity is alive and kicking! :P Lets continue with connecting the LCD to see if that works also. I used old ribbon cables found in computer to connect the LCD and a couple of other "external" components in this project. Before you plug in new components, make sure the USB cable is unplugged to avoid any shorts. The LCD wont show anything if the LCD contrast potentiometer is not connected. It's marked LCD-pot on the PCB.

The wiring order of the LCD potentiometer is as follows: [+5v - Ground - LCD-connector]. Keep this in mind when connecting the potmeter to the board. The outer two pins need to be connected to +5v and ground. The middle connector is connected to the 3rd pin on the PCB, so a little different than you might expect.

After you've connected the LCD and the potmeter it's time to see if they actually work. Just plug in the USB cable and check the LCD. If there's nothing on the screen, try adjusting the potmeter all the way to the other limit. If still nothing appears on the display, try flipping the LCD connector cable (after unplugging the USB). If then still nothing appears, you might have a faulty ribbon cable. After unplugging usb you can always use a multimeter to check connections.


<font color="red">[... WORK IN PROGRESS ...]</font>

## Step x: Disassembly of the FCB1010##
Disconnect all cables from the unit. Unscrew all the screws at the bottom of the unit and remove the bottom plate. Clip the green grounding cable that is connected to the bottom plate, we dont need it anymore since the power transformer will be housed externally. This is NOT optional because the power transformer is in the way for the new circuitry.

Basically you will need to modify the unit so Midi IN/OUT and power is transferred through one cable. The instructions for the recommended modification can be found here: [Behringer FCB1010 Power and Midi IN/OUT over single UTP / RJ45 Cable Modification](http://mackatackblog.wordpress.com/2012/06/15/behringer-fcb1010-power-and-midi-inout-over-single-utp-rj45-cable-modification/).

Once you have removed power transformer, take  out the old circuitboard with the status leds, led digits. Gently disconnect all the connectors from the board by removing the hot glue and pulling out the cables. Some connectors are directly soldered to the old board and need to be clipped off, you'll have to add new connectors to these cables later on.

