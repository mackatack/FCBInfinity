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
#include <EEPROM.h>

#include "utils_FCBSettings.h"

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

// Some important variables we'll need later on and want globally, if we want to
// access them from any file, externalize them in fcbinfinity.h

int     g_iCurrentPreset = 0;       // Here we track the current preset, in case we want to use it
int     g_iPresetBank = 0;          // The bank controllable by the BankUp and BankDown buttons
int     g_iAxeFxMidiChannel = 1;    // The channel the AxeFx expects to receive incoming messages
int     g_iStompBoxMode = 0;        // The current stompboxmode, see the loop() function below
boolean g_bXYToggle = false;        // The X/Y toggler

// The defines for the stompbox modes, see the switch clause below
#define STOMP_MODE_NORMAL     0
#define STOMP_MODE_10STOMPS   1
#define STOMP_MODE_LOOPER     2

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
  for(int i=0; i<=7; ++i) {
    ledControl.setDigit(0, i, 8, true);
  }
  Serial.println("- all leds on done");

  // Set the pins for the Stompbox bank RGB-led to output (see fcbinfinity.h for pinout)
  // the PIN_RGBLED_x pins are PWM pins, so an AnalogWrite() will set the led
  // intensity for this led.
  pinMode(PIN_RGBLED_R, OUTPUT);
  pinMode(PIN_RGBLED_G, OUTPUT);
  pinMode(PIN_RGBLED_B, OUTPUT);
  Serial.println("- rgb leds on done");

  // Perform the bootsplash animation
  doBootSplash();
  Serial.println("- bootsplash done");

  // Turn off the on-board teensy led to indicate that
  // the setup has completed
  digitalWrite(PIN_ONBOARD_LED, LOW);
  Serial.println("- setup() done");

  // First let's find out if the AxeFx has Midi Thru turned on.
  // We dont want our own messages thrown back at us.
  AxeMidi.sendLoopbackCheck();

  // Request the AxeFx to send us the preset name
  // Later on, we also want to trigger the update for all button states
  // and scroll to the correct, etc.
  AxeMidi.requestPresetName();

  // Set the bank to the initial presetbank, probably 0
  setLedDigitValue(g_iPresetBank);

  // Set the StompBoxMode to the normal mode, this just
  // sets the led to the correct color
  setStompBoxMode(STOMP_MODE_NORMAL);

} // setup()


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

  // Lets check if we received a MIDI message
  if (AxeMidi.hasMessage()) {
      // Yup we've got data, see AxeMidi.h and MIDI.h for more info
      // AxeMidi.getType()
      // AxeMidi.getData1()
      // AxeMidi.getData2();
      // AxeMidi.getSysExArray();

    if (AxeMidi.getType()==SysEx) {
      // Well well, someone (probably the AxeFx) is talking to us!
      // Keep in mind the any midi messages that we receive might
      // be an echo of a message that we sent ourselves
      handleMidiSysEx();
    }
  }

  // Play around with the expression pedals a little
  // ExpPedal1 has a new value?
  // Send some debug data over the serial communications, set the value on the LED-digits and send a midi message
  if (ExpPedal1.hasChanged()) {
    // Send CC# 1 on channel 1, for debugging
    AxeMidi.sendControlChange(1, ExpPedal1.getValue(), g_iAxeFxMidiChannel);
  }

  // ExpPedal2 has a new value?
  // Just send the midi message
  if (ExpPedal2.hasChanged()) {
    // Send CC# 2 on channel 1, for debugging
    AxeMidi.sendControlChange(2, ExpPedal2.getValue(), g_iAxeFxMidiChannel);
  }

  // button 5 on the lower row toggles X/Y, it does that in any mode, so we can just
  // leave this outside of the StompBoxMode logic
  if (btnRowLower[4].btn.fallingEdge()) {
    g_bXYToggle = !g_bXYToggle;
    AxeMidi.sendToggleXY(g_bXYToggle, g_iAxeFxMidiChannel);
    return;
  }

  // Right, since we're jumping into the stompbox-mode matter, let me quickly explain what
  // I'm trying to accomplish with this. Lets say you have a song where you're only using one preset
  // having 4 buttons to switch presets you wont even use is kind of useless, what if we could use those
  // to control effects as well. The code below allows us to do so.
  switch (g_iStompBoxMode) {

    case STOMP_MODE_NORMAL: {
      // This StompBoxMode is the normal one where the top 5 buttons toggle
      // some effects, the 1-4 bottom buttons control the preset changes and
      // the 5th button controls the X/Y switch. BankUp/Down control the bank
      // for the preset switching on the FCBInfinity

      // If the stompBox button is pressed, we want to switch to STOMP_MODE_10STOMPS
      // Just return, because all the remaining code below is useless now.
      // Later on, a long press will change us to looper mode
      if (btnStompBank.fallingEdge()) {
        setStompBoxMode(STOMP_MODE_10STOMPS);
        return;
      }

      // Button 1 to 4 on the bottom row send the ProgramChange message over midi to select new
      // presets on the AxeFx in this mode.
      // Please note that fallingEdge() detects when the button is pressed so this will react much
      // faster than some other devices, that only take action when a button is UNpressed, quite the difference.
      for(int i=0; i<4; ++i) {
        if (btnRowLower[i].btn.fallingEdge()) {
          // Send the PC message, i is the button number and add the current bank
          g_iCurrentPreset = i + 4*g_iPresetBank + 1;
          AxeMidi.sendPresetChange(g_iCurrentPreset, g_iAxeFxMidiChannel);
          return;
        }
      }

      // If the bankUp/Down buttons are pressed update the presetBank variable.
      // Later on we might want to flash preset switching buttons or something and have a feature where you
      // cycle through banks faster by keeping the button pressed
      // For now, KISS :P
      if (btnBankUp.fallingEdge()) {
        ++g_iPresetBank;
        if (g_iPresetBank>999) g_iPresetBank = 999;
        setLedDigitValue(g_iPresetBank);
        return;
      }
      if (btnBankDown.fallingEdge()) {
        --g_iPresetBank;
        if (g_iPresetBank<0) g_iPresetBank=0;
        setLedDigitValue(g_iPresetBank);
        return;
      }

      // @TODO: implement the stompbox button logic

    } // end of STOMP_MODE_NORMAL
    break;

    case STOMP_MODE_10STOMPS: {
      // In this mode the RGB-led is colored blue and all the upper and lower buttons
      // toggle effect bypasses, the BankUp/Down now just select the next or previous
      // preset.

      // If the stompBox button is pressed, we want to switch back to STOMP_MODE_NORMAL
      // Just return, because all the remaining code below is useless now.
      // Later on, a long press will change us to looper mode
      if (btnStompBank.fallingEdge()) {
        setStompBoxMode(STOMP_MODE_NORMAL);
        return;
      }

      // In this mode the bankUp/Down buttons react a little different; instead of changing banks
      // they will just move to the next or previous preset on the AxeFx. We'll calculate the new
      // bank automatically when we receive the preset changed SysEx from the AxeFx.
      if (btnBankUp.fallingEdge()) {
        AxeMidi.sendPresetChange(g_iCurrentPreset+1, g_iAxeFxMidiChannel);
        return;
      }
      if (btnBankDown.fallingEdge()) {
        AxeMidi.sendPresetChange(g_iCurrentPreset-1, g_iAxeFxMidiChannel);
        return;
      }

      // @TODO: implement the stompbox button logic

    } // end of STOMP_MODE_10STOMPS
    break;

    case STOMP_MODE_LOOPER: {
      // This mode changes the top buttons into the controls for the looper in the AxeFx
      // I will implement this later...

    } // end of STOMP_MODE_LOOPER
    break;

  }


  // ##############################
  // Some additional io debugging functions below

  // Update the button states, when a button is pressed set
  // the LED-Digits to the button id, toggle the indicator led
  // that is associated with the button
  for(int i=0; i<5; ++i) {
    // Check the upper row of buttons
    if (btnRowUpper[i].btn.fallingEdge()) {
      btnRowUpper[i].ledStatus = !btnRowUpper[i].ledStatus;
      ledControl.setLed(0, btnRowUpper[i].x, btnRowUpper[i].y, btnRowUpper[i].ledStatus);
      return;
    }

    // Check the lower row of buttons
    if (btnRowLower[i].btn.fallingEdge()) {
      btnRowLower[i].ledStatus = !btnRowLower[i].ledStatus;
      ledControl.setLed(0, btnRowLower[i].x, btnRowLower[i].y, btnRowLower[i].ledStatus);
      return;
    }
  }

} // loop()

/**
 * A seperate function to handle all the midi sysex messages
 * we might receive. Putting this in a separate function allows
 * us to use 'return' in case of errors etc.
 *
 * @TODO add more documentation and check the manufacturer code
 * in the SysEx to check if the message really came from the AxeFx
 */
void handleMidiSysEx() {
  int length = AxeMidi.getData1();
  // In case the length<5, it's an empty SysEx, just ignore it.
  if (length<5) return;

  // Get the byte array SysEx message
  byte *sysex = AxeMidi.getSysExArray();

  // Byte 5 of the SysEx message holds the function number
  switch (sysex[5]) {

    case SYSEX_AXEFX_REALTIME_TEMPO:
      // Tempo, just flash a led or something
      // There's no additional data
      return;

    case SYSEX_AXEFX_REALTIME_TUNER: {
      // Tuner
      // Byte 6 Holds the note starting at A
      // Byte 7 Holds the octave
      // Byte 8 Holds the finetune data between 0x00 and 0x7F

      lcd.setCursor(0,1);
      lcd.print("                    ");

      // Translate the finetune (0-127) to a value between 2 and 20
      // so we know where to print the '|' character on the lcd
      // We can initialize the pos integer here because i've added curly
      // braces around this case block.
      int pos = map(sysex[8], 0, 127, 2, 20);
      lcd.setCursor(pos,1);
      lcd.print("|");

      // Show a > or < if we need to tune up or down.
      // This will show >|< if we're in tune
      if (sysex[8]>=62)
        lcd.print("<");
      if (sysex[8]<=65) {
        lcd.setCursor(pos-1,1);
        lcd.print(">");
      }

      // Some debug info, print the raw finetune data
      lcd.setCursor(18,1);
      lcd.print(sysex[8], HEX);

      // Jump to the first character on the second line of the lcd
      // Show the note name here
      lcd.setCursor(0,1);
      lcd.print(AxeMidi.notes[sysex[6]]);

      return;
    }
    case SYSEX_AXEFX_PRESET_NAME:
      // Preset name response, print the preset name on the lcd
      // Echo? If patch name length <= 8, just return
      if (length<=8) return;

      // just output the sysex bytes starting from position 6 to the lcd
      lcd.setCursor(3,0);
      lcd.print(" ");
      for(int i=6; i<length && i<20+6-4; ++i) {
        lcd.print((char)sysex[i]);
      }

      return;

    case SYSEX_AXEFX_PRESET_CHANGE:
      // Patch change event!

      // Byte 6 and 7 hold the new patch number (starting at 0)
      // however these values only count to 127, so values higher
      // than 0x7F need to be calculated differently
      Serial.print("Patch change: ");
      g_iCurrentPreset = sysex[6]*128 + sysex[7] + 1;
      g_iPresetBank = (g_iCurrentPreset-1)/4;
      setLedDigitValue(g_iPresetBank);

      Serial.print(g_iCurrentPreset);
      Serial.println("!");

      // Put the new number on the LCD, but prefix the value
      // with zeros just like the AxeFx does.
      lcd.setCursor(0,0);
      if (g_iCurrentPreset<100)
        lcd.print("0");
      if (g_iCurrentPreset<10)
        lcd.print("0");
      lcd.print(g_iCurrentPreset);
      lcd.print(" ");

      // We know the preset number, now ask the AxeFx to send us
      // the preset name as well.
      AxeMidi.requestPresetName();

      // Also ask the AxeFx to send us the effect bypass states
      AxeMidi.requestBypassStates();

      return;

    case SYSEX_AXEFX_GET_PRESET_EFFECT_BLOCKS_AND_CC_AND_BYPASS_STATE:
      // We received the current states of the effects.
      // The sysex will contain the following data
      // see: http://forum.fractalaudio.com/other-midi-controllers/39161-using-sysex-recall-present-effect-bypass-status-info-available.html
      // 0xdd effect ID LS nibble
      // 0xdd effect ID MS nibble
      // 0xdd bypass CC# LS nibble
      // 0xdd bypass CC# MS nibble
      // 0xdd bypass state: 0=bypassed; 1=not bypassed

      // Some debug code until we figure out what all this means :P
      Serial.println("Preset effect states:");
      for(int i=6; i<length-4; i+=5) {
        Serial.print(" - Effect ");
        Serial.print(sysex[i]);
        Serial.print(" ");
        Serial.print(sysex[i+1]);
        Serial.print(", cc");
        Serial.print(sysex[i+2]);
        Serial.print(" ");
        Serial.print(sysex[i+3]);
        Serial.print(", state: ");
        Serial.print(sysex[i+4]);
        Serial.println(".");
      }
      Serial.println("End effect states");

      break;

    case SYSEX_LOOBACK_CHECK_DATA:
      // Oof, midi thru is enabled on the AxeFx, send a message to the user that they
      // need to disable it.
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("#WARNING# Disable   Midi Thru on AxeFx");
      delay(5000);
      lcd.clear();
      lcd.setCursor(0,0);

      return;
  }

  // Some debugging code to just dump the sysex data on the serial line.
  // This is only reached when the above code doesn't handle the sysex
  char buffer[4];
  Serial.print("Sysex HEX: ");
  for(int i=0; i<length; ++i) {
    Serial.print(sysex[i], HEX);
    Serial.print(" ");
  }
  Serial.println(" ");
}

/**
 * Changes the current StompBoxMode and lights up the RGB-led
 * to set the current mode. See the loop() function for more info
 */
void setStompBoxMode(int iNewMode) {
  // set the new mode
  g_iStompBoxMode = iNewMode;

  // Change the RGB led to reflect the new mode.
  switch (g_iStompBoxMode) {
    case STOMP_MODE_NORMAL:
      setRGBLed(0, 0, 0);
      lcd.setCursor(0,1);
      lcd.print("Mode: Normal        ");
      break;
    case STOMP_MODE_10STOMPS:
      setRGBLed(0, 0, 50);
      lcd.setCursor(0,1);
      lcd.print("Mode: 10 stomps     ");
      break;
    case STOMP_MODE_LOOPER:
      setRGBLed(50, 0, 0);
      lcd.setCursor(0,1);
      lcd.print("Mode: Looper        ");
      break;
  }
}

/**
 * Sets the RGB-Led to a new color.
 */
void setRGBLed(int r, int g, int b) {
  analogWrite(PIN_RGBLED_R, r);
  analogWrite(PIN_RGBLED_G, g);
  analogWrite(PIN_RGBLED_B, b);
}


/**
 * updateIO
 * This method needs to be called first every loop and only once every loop
 * It checks for new data on all the IO, such as buttons, analog devices (ExpPedals) and Midi
 */
void updateIO() {
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
  btnExpPedalRight.update();
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
  lcd.print("  FCBInfinity v1.0");
  lcd.print("      by Mackatack");

  delay(800);
  lcd.clear();
  lcd.setCursor(0,0);
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
