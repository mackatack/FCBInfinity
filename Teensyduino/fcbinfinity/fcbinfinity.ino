/**
 * FCBInfinity, a Behringer FCB1010 modification.
 *
 * This is the main Arduino/Teensyduino file for the FCBInfinity project.
 * Please note that this is VERY much still a work in progress and you should
 * consider the code below still in Alpha/Testing phase. Use at work risk
 *
 * -Mackatack
 */


/**
 * ###########################################################
 * Main includes for the FCBInfinity project
 */

#include <Bounce.h>
#include <LedControl.h>
#include <LiquidCrystalFast.h>
#include <MIDI.h>


#include "io_ExpPedals.h"
#include "io_AxeMidi.h"

#include "fcbinfinity.h"

/**
 * ###########################################################
 * Initialization of the objects that are available throughout
 * the entire project. see fcbinfinity.h for more information
 * per object, this is also where these objects are externalized
 */

// Initialization of the LCD
LiquidCrystalFast lcd(19, 18, 38, 39, 40, 41, 42);  // pins: (RS, RW, Enable, D4, D5, D6, D7)

// Initialization of the MAX7219 chip, this chip controls all the leds on the system (except stompbank-rgb)
LedControl ledControl = LedControl(43, 20, 21, 1);  // pins: (DIN, CLK, LOAD, number_of_chips)

// The buttons in an array together with the x,y coordinates where the corresponding
// led can be found on the MAX7219 chip using the ledControl object.
// See the mkBounce() function for more information about the buttons.
_FCBInfButton btnRowUpper[] = {
  {2, 4, mkBounce(11), false},
  {2, 6, mkBounce(12), false},
  {2, 3, mkBounce(13), false},
  {2, 7, mkBounce(14), false},
  {2, 5, mkBounce(15), false}
};
_FCBInfButton btnRowLower[] = {
  {1, 4, mkBounce(4), false},
  {1, 6, mkBounce(5), false},
  {1, 3, mkBounce(7), false},
  {1, 7, mkBounce(8), false},
  {1, 5, mkBounce(9), false}
};
Bounce btnBankUp = mkBounce(16);
Bounce btnBankDown = mkBounce(10);
Bounce btnStompBank = mkBounce(25);
Bounce btnExpPedalRight = mkBounce(24);

// Initialization of the ExpressionPedals
ExpPedals_Class ExpPedal1(A6);  // Analog pin 6 (pin 44 on the Teensy)
ExpPedals_Class ExpPedal2(A7);  // Analog pin 7 (pin 45 on the Teensy)


/**
 * ###########################################################
 * setup()
 * This function gets called once during the entire startup
 * of the device. Here we set all the pin modes and start the Midi
 * interface. This is also the place where we start our little
 * boot animation on the LCD.
 */
void setup() {
  // Light up the on-board teensy led
  pinMode(PIN_ONBOARD_LED, OUTPUT);
  digitalWrite(PIN_ONBOARD_LED, HIGH);  // HIGH means on, LOW means off

  // Start the debugging information over serial
  Serial.begin(57600);
  Serial.println("FCBInfinity Startup");

  // Initialize MIDI, for now set midiThru off and channel to OMNI
  AxeMidi.begin(MIDI_CHANNEL_OMNI);
  AxeMidi.turnThruOff();
  // The AxeFX-II wants checksummed sysex messages, set this to false
  // if you use an Ultra or older model.
  AxeMidi.setSendReceiveChecksummedSysEx(true);
  Serial.println("- midi setup done");

  // Turn all the leds on that are connected to the MAX chip.
  ledControl.shutdown(0, false);  // turns on display
  ledControl.setIntensity(0, 4);  // 15 = brightest
  for(int i=0; i<=7; i++) {
    ledControl.setDigit(0, i, 8, true);
  }
  Serial.println("- all leds on done");

  // Set the pins for the Stompbox bank RGB-led to output (see fcbinfinity.h for pinout)
  // the PIN_RGBLED_x pins are PWM pins, so an AnalogWrite() will set the led
  // intensity for this led.
  pinMode(PIN_RGBLED_R, OUTPUT); analogWrite(PIN_RGBLED_R, 50);
  pinMode(PIN_RGBLED_G, OUTPUT); analogWrite(PIN_RGBLED_G, 50);
  pinMode(PIN_RGBLED_B, OUTPUT); analogWrite(PIN_RGBLED_B, 50);
  Serial.println("- rgb leds on done");

  // Perform the bootsplash animation
  doBootSplash();
  Serial.println("- bootsplash done");

  // Turn off the on-board teensy led to indicate that
  // the setup has completed
  digitalWrite(PIN_ONBOARD_LED, LOW);
  Serial.println("- setup() done");
}


/**
 * ###########################################################
 * loop()
 * This function gets called repeatedly while the device is active
 * this is where we check for button presses and midi data, this is
 * also where we update the displays, leds and send midi.
 */
void loop() {

  // Check all the connected inputs; buttons, expPedals, MIDI, etc. for new data or state changes
  updateIO();

  // Play around with the expression pedals a little
  // ExpPedal1 has a new value?
  // Send some debug data over the serial communications, set the value on the LED-digits and send a midi message
  if (ExpPedal1.hasChanged()) {
    Serial.print("ExpPedal1: ");
    Serial.print(ExpPedal1.getValue());
    Serial.print(", raw: ");
    Serial.println(ExpPedal1.getRawValue());
    setLedDigitValue(ExpPedal1.getValue());
    AxeMidi.sendControlChange(1, ExpPedal1.getValue(), 1);
  }

  // ExpPedal2 has a new value?
  // Just send the midi message
  if (ExpPedal2.hasChanged()) {
    AxeMidi.sendControlChange(2, ExpPedal2.getValue(), 1);
  }

  // Lets check if we received a MIDI message
  if (AxeMidi.hasMessage()) {
      // Yup we've got data, see AxeMidi.h and MIDI.h for more info
      // AxeMidi.getType()
      // AxeMidi.getData1()
      // AxeMidi.getData2();
      // AxeMidi.getSysExArray();
  }

  // Update the button states
  for(int i=0; i<5; ++i) {
    if (btnRowUpper[i].btn.fallingEdge()) {
      setLedDigitValue(i+10+1);
      btnRowUpper[i].ledStatus = !btnRowUpper[i].ledStatus;
      ledControl.setLed(0, btnRowUpper[i].x, btnRowUpper[i].y, btnRowUpper[i].ledStatus);
    }
    if (btnRowLower[i].btn.fallingEdge()) {
      setLedDigitValue(i+20+1);
      btnRowLower[i].ledStatus = !btnRowLower[i].ledStatus;
      ledControl.setLed(0, btnRowLower[i].x, btnRowLower[i].y, btnRowLower[i].ledStatus);
    }
  }

  if (btnBankUp.fallingEdge())
    setLedDigitValue(19);
  if (btnBankDown.fallingEdge())
    setLedDigitValue(29);

  static int iStompBank = 0;

  if (btnStompBank.fallingEdge()) {
    // Stomp button has been pressed
    iStompBank++;
    iStompBank %= 3;
    analogWrite(PIN_RGBLED_R, 0);
    analogWrite(PIN_RGBLED_G, 0);
    analogWrite(PIN_RGBLED_B, 0);
    switch (iStompBank) {
      case 0:
        analogWrite(PIN_RGBLED_R, 60); break;
      case 1:
        analogWrite(PIN_RGBLED_G, 60); break;
      case 2:
        analogWrite(PIN_RGBLED_B, 60); break;
    }

    static boolean stompTestToggle;
    stompTestToggle = !stompTestToggle;
    for(int i=0; i<=7; i++) {
      if (stompTestToggle)
        ledControl.setDigit(0, i, 8, true);
      else
        ledControl.setChar(0, i, ' ', false);
    }

    // Also send a sysEx to the AxeFX to request the patch name
    AxeMidi.requestPresetName();
  }


  return;


  // A piece of testing code that just toggles the leds
  // on the device every 100ms
  static elapsedMillis digitElapsed;
  static boolean noDigitStatus;
  // 100ms passed?
  if (digitElapsed>100) {
    // reset timer and flip the led status
    digitElapsed = 0;
    noDigitStatus = !noDigitStatus;
    // 0, 1, 2 control the led digits, so start at 3.
    for(int i=3; i<=7; i++) {
      if (noDigitStatus)
        ledControl.setDigit(0, i, 8, true);
      else
        ledControl.setChar(0, i, ' ', false);
    }
  }



  /*
  // Keeping this here for a little while, until we move it elsewhere
  for (int i=0; i<5; ++i) {
    btnRowUpper[i].btn.update();
    if (btnRowUpper[i].btn.fallingEdge()) {
      lcd.setCursor(0,0);
      lcd.print(i);
      btnRowUpper[i].ledStatus = !btnRowUpper[i].ledStatus;
      ledControl.setLed(0, btnRowUpper[i].y, btnRowUpper[i].x, btnRowUpper[i].ledStatus);
    }

    btnRowLower[i].btn.update();
    if (btnRowLower[i].btn.fallingEdge()) {
      lcd.setCursor(0,1);
      lcd.print(i);
      btnRowLower[i].ledStatus = !btnRowLower[i].ledStatus;
      ledControl.setLed(0, btnRowLower[i].y, btnRowLower[i].x, btnRowLower[i].ledStatus);
    }
  }*/

}


/**
 * updateIO
 * This method needs to be called first every loop and only once every loop
 * It checks for new data on all the IO, such as buttons, analog devices (ExpPedals) and Midi
 */
elapsedMicros updateIOTimer;
void updateIO() {
  // Debug: output the time it took to do all the other stuff and
  // start the IO handling again.
  Serial.print("IO Update started after micros: ");
  Serial.println(updateIOTimer);

  // Reset the timer
  updateIOTimer = 0;

  // Check for new midi messages
  AxeMidi.handleMidi();

  // Update the states of the onboard expression pedals
  ExpPedal1.update();
  ExpPedal2.update();

  // Update the button states
  for(int i=0; i<5; ++i) {
    btnRowUpper[i].btn.update();
  }
  for(int i=0; i<5; ++i) {
    btnRowLower[i].btn.update();
  }
  btnBankUp.update();
  btnBankDown.update();
  btnStompBank.update();

  // Debug: output the time it took to update all the IO
  Serial.print("IO Update took micros: ");
  Serial.println(updateIOTimer);
}

/**
 * setLedDigitValue
 * sets a integer value on the led-digits
 */
void setLedDigitValue(int value) {
  boolean lastDP = false;
  boolean firstDP = false;
  if (value>999)
    lastDP = true;
  else if (value<0)
    firstDP = true;

  ledControl.setDigit(0, 2, value%10, lastDP);
  ledControl.setDigit(0, 1, value/10%10, false);
  ledControl.setDigit(0, 0, value/100%10, firstDP);
}

/**
 * doBootSplash()
 * My little FCBInfinity bootsplash animation,
 * This allows me to quickly see if the displays and leds are working
 * during startup.
 * This will probably get moved to an external file anytime soon.
 */
void doBootSplash() {
  // Set the led digits to read "InF."
  ledControl.setChar(0, 0, '1', false);
  ledControl.setChar(0, 1, ' ', false);
  ledControl.setLed(0, 1, 3, true);
  ledControl.setLed(0, 1, 5, true);
  ledControl.setLed(0, 1, 7, true);
  ledControl.setChar(0, 2, 'F', true);

   // Write something on the LCD
  lcd.begin(20, 2);
  lcd.println(" FCB-Infinity v1.0");
  lcd.println("         Mackatack");
  /*
  delay(800);
  lcd.clear();
  lcd.setCursor(0,0);
  */
}

/**
 * Initialization function for all the buttons in this project;
 * it sets the corresponding pins to the INPUT_PULLUP state.
 * This state uses an internal pull-up resistor in the AT90USB1286 chip
 * This is why we didn't need to add a resistor per button to the custom
 * PCB. <3 Teensy/Arduino
 * @returns The Bounce object to check the button states
 */
Bounce mkBounce(int pin) {
  pinMode(pin, INPUT_PULLUP);
  return Bounce(pin, 10);
}
