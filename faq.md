# FCBInfinity Frequently Asked Questions #
Below you can find a list of frequently asked questions for the FCBInfinity modification for the Behringer FCB1010 midi-footpedal.

## Why a Teensy board and not Arduino for example? ##
A Teensy board has a pull-up resistor built in for the push buttons, that way I didn't have to add a resistor for every button on the footpedal or do crazy multiplexing tricks. Also, the Teensy is very small, so much easier to integrate directly onto a PCB.

## Can I still program the device using Ripwerx or manually by using the buttons? ##
NO, the Teensy chip will completely replace the original chip and programming. So unless you program this functionality yourself into the Teensy you will not be able to change the programming on the road by pressing the buttons or using any software like Ripwerx or the FCB1010 editor. For now, any changes in the programming will require you to have a computer, the Arduino/Teensyduino SDK, edit the source code, compile and upload the new program to your Teensy. It sounds complicated, but it's nothing more than hooking up your FCBInfinity to your computer via USB, edit some code and hitting "Upload".

## I would like a FCBInfinity, can you modify my unit for me? ##
At the moment I really cannot help you with the modification of your unit other than answering questions online. All the soldering, hacking, sawing, etc. is up to you. This is a pretty big project so please consult yourself if you're willing to invest all this work. Unless you're pro it'll take you a couple of days to finish the mod, so make sure you dont have any gigs planned in the next couple of days.

## Ok, I'll do it myself, where can I get your PCB? ##
I currently dont sell the PCB. You can get the schematics and toner transfer images to make your own printed circuit board (PCB). I've also created a board myself, but I can honestly tell you it's no fun job. There are many PCB Prototyping companies that can create the board for you, often for about 40 bucks. On the github you can find the files required to have the board created for you (in Gerber format). If you're looking for a price quote, the board dimensions are 160x40 mm, double layer.

## What other components do I need? ##
You can find a partlist here: [FCBInfinity partlist](https://github.com/mackatack/FCBInfinity/blob/master/Partlist.md)

## Whats that link to your blog again? ##
[http://mackatackblog.wordpress.com](http://mackatackblog.wordpress.com)