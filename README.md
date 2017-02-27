BLEBriteLamp
============

Arduino software to use with the BriteLamp iOS app.  A Bluetooth Low Energy connection from an iOS device selects colors to display on a chain of ShiftBrites.  The colors will cycle through all the LEDs in the chain.

The BriteLamp app lets you select the number of LEDs in the pattern and their colors. It also controls the speed of the color changes.  Once set up, you use the iOS app to download the selection to the Arduino. The Arduino pushes the color pattern through the chain of Shiftbrites.

The result is a set of lights you can configure for the holiday of your choice - create some themes and have fun! 

_Note_: As of early 2017, Sparkfun no longer stock the Shiftbrite RGB LEDs I used for this project. I haven't investigated alternatives (the Shiftbrite combined a shift register with a RGB LED).


What you'll need
---------

BriteLamp can be the basis for an interesting / creative project using connected lighting. The basics you'll need are:

* An iOS device (iPad, iPod or iPhone) running iOS 8 (or better) to control the lights.
* The BriteLamp app.
* An Arduino processor with a BLE (Bluetooth Low Energy) shield, or built-in BLE support, and a computer to use to program it (with cable).
* Arduino SDK, downloaded from http://arduino.cc. This project used version 1.0.5.
* A power source for the Arduino.
* The BriteLamp Arduino code, downloaded from http://github.com/kamorris/BLEBriteLamp
* At least one ShiftBrite(TM) RGB LED.
* Cables for connecting the Arduino and the ShiftBrite. I used the standard cables that were available from SparkFun.
* (Optional) an enclosure or lightbox of some sort for the ShiftBrites.


For more information about this code, see http://laceworkssoftware.com/BriteLamp.html.  The description includes a video of the Arduino and iOS software working together, and 
The iOS app is available from https://itunes.apple.com/us/app/britelamp/id946727057?ls=1&mt=8
