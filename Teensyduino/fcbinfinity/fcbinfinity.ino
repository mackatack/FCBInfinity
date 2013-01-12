/**
 * FCBInfinity, a Behringer FCB1010 modification.
 *
 * This is the main Arduino/Teensyduino file for the FCBInfinity project.
 * Please note that this is VERY much still a work in progress and you should
 * consider the code below still in Alpha/Testing phase. Use at own risk
 *
 * For more info on all the Arduino functions, check:
 * http://arduino.cc/en/Reference/HomePage
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
#include <EEPROM.h>
#include "io_MIDI.h"
#include "io_AxeMidi.h"
#include "io_ExpPedals.h"
#include "utils_FCBSettings.h"
#include "utils_FCBTimer.h"
#include "utils_FCBEffectManager.h"
#include "modes_DefaultMode.h"

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
Bounce btnStompMode = mkBounce(25);
Bounce btnStompModeLong = mkBounce(1500);
Bounce btnExpPedalRight = mkBounce(24);
Bounce btnTapTempoTuner = mkBounce(22);

_FCBInfButton btnExpPedalRightS = {1, 4, btnExpPedalRight, false};

// Initialization of the ExpressionPedals
ExpPedals_Class ExpPedal1(A6);  // Analog pin 6 (pin 44 on the Teensy)
ExpPedals_Class ExpPedal2(A7);  // Analog pin 7 (pin 45 on the Teensy)

// The defines for the stompbox modes, see the switch clause in the loop() below
#define STOMP_MODE_NORMAL     0
#define STOMP_MODE_10STOMPS   1
#define STOMP_MODE_LOOPER     2
#define STOMP_MODE_SCENE      3

// Some important variables we'll need later on and want globally, if we want to
// access them from any file, externalize them in fcbinfinity.h
int     g_iCurrentPreset = 0;                     // Here we track the current preset, in case we want to use it
int     g_iCurrentScene = 0;                      // Here we track the current scene for the current preset.
int     g_iPresetBank = 0;                        // The bank controllable by the BankUp and BankDown buttons
int     g_iStompBoxMode = STOMP_MODE_10STOMPS;    // The current stompboxmode, see the loop() function below

// Pointers to the currently active and previously active mode
FCBMode * g_pCurrentMode = NULL;
FCBMode * g_pPreviousMode = NULL;

/**
 * ###########################################################
 * The main functions of the FCBInfinity project
 */

// A timer callback to turn off the taptempo led again
void timerTapTempoLedOff(FCBTimer*) {
  // Just turn off the green channel of the rgb led again.
  analogWrite(PIN_RGBLED_G, 0);
}

/**
 * These two functions will be called by the AxeMidi library because we registered
 * them as a callback in setup()
 */
void onAxeFxConnectedEvent() {
  // The AxeMidi library has received the model number from
  // the AxeFx, now we know whether or not we should add checksums to SysEx
  // messages and how to parse specific responses.
  // Lets ask the AxeFx to send us the current preset number, once
  // the AxeFx responds we automatically request the PresetName and
  // effect states, see onAxeFxSysExMessage()
  lcd.setCursor(0,0);
  lcd.print("    Loading data... ");
  AxeMidi.requestPresetNumber();
}
void onAxeFxDisconnectedEvent() {
  lcd.setCursor(0,0);
  lcd.print(" !Axe Disconnected! ");
}

/**
 * This timer callback requests the new effect states every second
 */
void timerUpdateEffectStates(FCBTimer*) {
  if (!AxeMidi.isInitialized()) return;
  FCBEffectManager.setStatesStale();
}

/**
 * This function will be called by the AxeMidi library because we registered
 * it as a callback in setup()
 * @param sysex holds the entire SysEx message including the opening and closing bytes
 * @param length holds the number bytes in the entire SysEx message
 */
void onAxeFxSysExMessage(byte * sysex, int length) {

  // Byte 5 of the SysEx message holds the function number, the function
  // ids are defined in AxeMidi.h
  switch (sysex[5])
  {
    case SYSEX_AXEFX_REALTIME_TEMPO:
      // Tempo, just flash a led or something
      // There's no additional data

      // Turn on the taptempo led, lets just use the green channel of the rgb led
      analogWrite(PIN_RGBLED_G, 80);

      // Schedule a new timer that turns off the led again after 150ms
      FCBTimerManager::addTimeout(150, &timerTapTempoLedOff);
      return;

    case SYSEX_AXEFX_PRESET_MODIFIED:
      // This gets sent when you manually edit the preset on the axefx, it sends this
      // when adding or removing effect blocks, editing parameters, etc. etc. almost everything.
      // Everything but when you manually bypass a block. Total suckage. The AxeFx is completely silent
      // when you press the "FX BYP" button on the Axe.

      // I guess we should use this later on to update params from the AxeFx, but we might as well ask for state
      // updates manually every few seconds using a timer, that is, if we really want to keep the device completely
      // in sync with the AxeFx (which we probably want, no? :P)

    case SYSEX_AXEFX_REALTIME_TUNER: {
      // Tuner
      // Byte 6 Holds the note starting at A
      // Byte 7 Holds the octave
      // Byte 8 Holds the finetune data between 0x00 and 0x7F
      lcd.setCursor(0,1);
      lcd.print("                    ");

      // Translate the finetune (0-127) to a value between 0 and 18*5 horizontal lines
      // so we know where to print the '|' character on the lcd
      // We can initialize the pos integer here because i've added curly
      // braces around this case block.
      int line = map(sysex[8], 0, 127, 0, 17*5);
      int pos = floor(line/5) + 2;

      // Show a > or < if we need to tune up or down.
      // This will show >|< if we're in tune
      if (sysex[8]<=64) {
        lcd.setCursor(11,1);
        lcd.print("<");
      }
      if (sysex[8]>=62) {
        lcd.setCursor(9,1);
        lcd.print(">");
      }

      lcd.setCursor(pos,1);
      lcd.write(line % 5);

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

      // Also ask the AxeFx to send us the effect bypass states
      AxeMidi.requestBypassStates();

      // We know the preset number and effect states, now ask the AxeFx to send us
      // the preset name as well.
      AxeMidi.requestPresetName();

      return;

    case SYSEX_AXEFX_SCENE_STATUS:

      // Scene status message
      if (g_iCurrentScene != sysex[6])
        g_iCurrentScene = sysex[6];

      break;

    case SYSEX_AXEFX_GET_PRESET_EFFECT_BLOCKS_AND_CC_AND_BYPASS_STATE:
      // We received the current states of the effects.
      // The sysex will contain the following data
      // see: http://forum.fractalaudio.com/other-midi-controllers/39161-using-sysex-recall-present-effect-bypass-status-info-available.html

      // Some debug code until we figure out what all this means :P
      //Serial.println("Preset effect states received");

      // declare some vars we'll use in the loop
      int state;
      int effectID;
      int cc;

      // Reset all the effectstates to 'not placed'
      FCBEffectManager.resetStates();

      for(int i=6; i<length-4; i+=5) {

        if (AxeMidi.getModel() < AXEMODEL_AXEFX2) {
          // Older models
          // byte+0 effect ID LS nibble
          // byte+1 effect ID MS nibble
          // byte+2 bypass CC# LS nibble
          // byte+3 bypass CC# MS nibble
          // byte+4 bypass state: 0=bypassed; 1=not bypassed
          effectID = (sysex[i+1] << 4) | sysex[i];
          cc = (sysex[i+3] << 4) | sysex[i+2];
          state = sysex[i+4];
        }
        else {
          // AxeFx3 and up style
          // for each effect there are 5 bytes:
          // byte+0, bit 1 = bypass, bit 2 is X/Y state
          // byte+1: 7 bits ccLs + 1 empty bit
          // byte+2: 6 empty bits + 1 bit for cc pedal or cc none + 1 bit ccMs
          // byte+3: 5 bits effectIdLs + 3 empty bits
          // byte+4: 4 empty bits + 4 bits effectIdMs
          effectID = ((sysex[i+3] & 0x78) >> 3) + ((sysex[i+4] & 0x0F) << 4); // block id
          cc = ((sysex[i+1] & 0x7E) >> 1) + ((sysex[i+2] & 3) << 6);  // cc number
          state = sysex[i] & 0x7F; // byp and XY state
        }

        // Update the state and cc of the effectblock to the new values.
        FCBEffectManager[effectID]->setStateAndCC(state, cc);
      }

      return;
  } // switch (sysex[5]) // FunctionID
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
      // No led
      setRGBLed(0, 0, 0);
      lcd.setCursor(0,1);
      lcd.print("Mode: Normal        ");
      break;
    case STOMP_MODE_10STOMPS:
      // Blue led
      setRGBLed(0, 0, 50);
      lcd.setCursor(0,1);
      lcd.print("Mode: 10 stomps     ");
      break;
    case STOMP_MODE_LOOPER:
      // Red led
      setRGBLed(50, 0, 0);
      lcd.setCursor(0,1);
      lcd.print("Mode: Looper        ");
      AxeMidi.requestLooperUpdates();
      break;
    case STOMP_MODE_SCENE:
      // purple led
      setRGBLed(50, 0, 50);
      lcd.setCursor(0,1);
      lcd.print("Mode: Scene select  ");
      break;
  }
}

/**
 * Sets the RGB-Led to a new color.
 * @param r,g,b: a range between 0 and 255. Please note that 255 will
 * burn out your led because there is no resistor between the port and the
 * led. Please use values between 0 and 130 to keep your led alive.
 */
inline void setRGBLed(int r, int g, int b) {
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
  btnStompMode.update();
  btnStompModeLong.update();
  btnExpPedalRight.update();
  btnTapTempoTuner.update();

  // That's currently all the input we have, all done.
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

  // Wait 800 ms and clear the LCD.
  delay(800);
  setLedDigitValue(0);
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
inline Bounce mkBounce(int pin) {
  // For arduino boards pinMode must be set to INPUT and a digitalWrite(HIGH) is required to
  // enable the internal resistor, for teensy the INPUT_PULLUP does all this for us.
  pinMode(pin, INPUT_PULLUP);
  return Bounce(pin, 30);
}

/**
 * This is a little bit more tricky function of FCBInfinity. The WAH effect stomp button
 * control whether the Exp1 pedal controls the WAH or the Pitch1.
 */
void handleWahPitchToggle(_FCBInfButton* btnObj) {

  // Set the led to reflect the bypass state of the Pitch1
  btnObj->setLed(FCBEffectManager[AXEFX_EFFECTID_Pitch1]->isActive());

  if (FCBEffectManager[AXEFX_EFFECTID_Pitch1]->isActive()) {
    // Pitch1 is enabled, lets add the behavior in this state.
    // Exp1 Controls Pitch1 via Control2

    // Button pressed? Disable Pitch1, that's it :P
    if (btnObj->btn.fallingEdge()) {
      FCBEffectManager[AXEFX_EFFECTID_Pitch1]->toggleActive();
      btnObj->setLed(false);
      return;
    }
  }
  else {
    // Pitch1 is disabled, Exp1 Controls WAH via Control1

    // Button pressed? Send Control1=0, enable Pitch1 and send Control2=0
    if (btnObj->btn.fallingEdge()) {
      AxeMidi.sendControlChange(AXEFX_DEFAULTCC_External_Control_1, 0);
      FCBEffectManager[AXEFX_EFFECTID_Pitch1]->toggleActive();
      AxeMidi.sendControlChange(AXEFX_DEFAULTCC_External_Control_2, 0);
      return;
    }
  }
}

/**
 *
 */
inline void setScene(int iNewSceneNumber) {
  g_iCurrentScene = iNewSceneNumber;
  FCBEffectManager.setStatesStale();
  AxeMidi.sendControlChange(34, iNewSceneNumber);

}

/**
 * This little function makes it easier for us to bind a button and associated led to a specific effect
 * on the AxeFx and allows us to define a fallback effect in case the given effect is not used, for Example:
 * handleEffectStompButton(btnRowLower[0], AXEFX_EFFECTID_Reverb1, AXEFX_EFFECTID_Reverb2);
 * lets the first button on the lower row handle the Reverb1 effect, but if that's not placed let it handle Rev2.
 * In case both effects are not placed the led will be off. If the targetted effect is placed and active the led
 * will be on.
 */
inline boolean handleEffectStompButton(_FCBInfButton* btnInfo, int effectID, int fallbackEffectID=-1, int fallbackEffectID2=-1) {
  if (!FCBEffectManager[effectID]->isPlaced())
    effectID = fallbackEffectID;
  if (!FCBEffectManager[effectID]->isPlaced())
    effectID = fallbackEffectID2;

  if (effectID<0 || !FCBEffectManager[effectID]->isPlaced()) {
    // Main effect or fallback not placed; turn off led and return
    btnInfo->setLed(false);
    return false;
  }

  // If the button is pressed, toggle the effect, update the led and
  // return false;
  if (btnInfo->btn.fallingEdge()) {
    FCBEffectManager[effectID]->toggleActive();
    btnInfo->setLed(FCBEffectManager[effectID]->isActive());
    return true;
  }

  // Button not pressed, just update the led and return false;
  btnInfo->setLed(FCBEffectManager[effectID]->isActive());
  return false;
}

/**
 * ###########################################################
 * setup()
 * This function gets called once during the entire startup
 * of the device. Here we set all the pin modes and start the Midi
 * interface. This is also the place where we start our little
 * boot animation on the LCD.
 */
byte tunerchars[5][8] = {
  {B10000,B10000,B10000,B10000,B10000,B10000,B10000},
  {B01000,B01000,B01000,B01000,B01000,B01000,B01000},
  {B00100,B00100,B00100,B00100,B00100,B00100,B00100},
  {B00010,B00010,B00010,B00010,B00010,B00010,B00010},
  {B00001,B00001,B00001,B00001,B00001,B00001,B00001}
};
void setup() {
  // Light up the on-board teensy led
  pinMode(PIN_ONBOARD_LED, OUTPUT);
  digitalWrite(PIN_ONBOARD_LED, HIGH);  // HIGH means on, LOW means off

  // Start the debugging information over serial
  Serial.begin(57600);
  Serial.println("FCBInfinity Startup");

  // Create some custom characters for the tuner
  lcd.createChar(0, tunerchars[0]);
  lcd.createChar(1, tunerchars[1]);
  lcd.createChar(2, tunerchars[2]);
  lcd.createChar(3, tunerchars[3]);
  lcd.createChar(4, tunerchars[4]);

  // Initialize MIDI, for now set midiThru off and channel to OMNI
  MIDINEW.begin(MIDI_CHANNEL_OMNI);
  MIDINEW.turnThruOff();
  AxeMidi.registerAxeSysExReceiveCallback(&onAxeFxSysExMessage);
  AxeMidi.registerAxeFxConnectedCallback(&onAxeFxConnectedEvent);
  AxeMidi.registerAxeFxDisconnectedCallback(&onAxeFxDisconnectedEvent);
  Serial.println("- midi setup done");

  // Setup the calibration for the ExpressionPedals
  ExpPedal1.setCalibration(32,824);
  ExpPedal2.setCalibration(28,946);

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

  // Set the bank to the initial presetbank, probably 0
  setLedDigitValue(g_iPresetBank);

  // Set the StompBoxMode to the normal mode, this just
  // sets the led to the correct color
  setStompBoxMode(STOMP_MODE_10STOMPS);

  // Just print a message on the LCD that we're waiting for messages...
  lcd.setCursor(0,0);
  lcd.print(" Waiting for AxeFX! ");

  FCBTimerManager::addInterval(1000, &timerUpdateEffectStates);
} // setup()

/**
 * ###########################################################
 * loop()
 * This function gets called repeatedly while the device is active
 * this is where we check for button presses and midi data, this is
 * also where we update the displays, leds and send midi.
 * Never ever use delay or very slow functions in this loop because
 * it will slow down your input handling tremendously.
 */
void loop() {

  // Check all the connected inputs; buttons, expPedals, MIDI, etc. for new data or state changes
  updateIO();

  // Process all the timers in our FCBTimerManager, this checks if we
  // have registered a timeout anywhere in the project and calls the
  // callback function if the timeout has expired. This is a neat way
  // to flash leds and do some repeated actions, without all the hassle.
  static elapsedMicros timerUpdate;
  if (timerUpdate>5000) {
    // There's no need to run the timers every single loop though,
    // only call it every ms.
    timerUpdate = 0;
    FCBTimerManager::processTimers();
  }

  // Lets see if the effectmanager states need updating
  FCBEffectManager.updateIfStale();

  // Right, since we're jumping into the stompbox-mode matter, let me quickly explain what
  // I'm trying to accomplish with this. Lets say you have a song where you're only using one preset
  // having 4 buttons to switch presets you wont even use is kind of useless, what if we could use those
  // to control effects as well. The code below allows us to do so.

  // Remember the oldStompBoxMode should we require it (return from looper mode)
  // But first lets check for a button press longer than 1 second, which will bring us to STOMP_MODE_10STOMPS
  static int l_iMainStompBoxMode = STOMP_MODE_10STOMPS;
  static bool btnStompLongPressedHandled = false;
  if (!btnStompLongPressedHandled && (btnStompMode.read() == LOW) && (btnStompMode.duration()>=1000)) {
    if (l_iMainStompBoxMode == STOMP_MODE_NORMAL)
      l_iMainStompBoxMode = STOMP_MODE_10STOMPS;
    else
      l_iMainStompBoxMode = STOMP_MODE_NORMAL;
    setStompBoxMode(l_iMainStompBoxMode);
    btnStompLongPressedHandled = true;
    return;
  }
  if (btnStompMode.fallingEdge()) {
    btnStompLongPressedHandled = false;
  }

  // Lets see what mode we're in
  switch (g_iStompBoxMode) {

    case STOMP_MODE_NORMAL: {
      // This StompBoxMode is the normal one where the top 5 buttons toggle
      // some effects, the 1-4 bottom buttons control the preset changes and
      // the 5th button controls the X/Y switch. BankUp/Down control the bank
      // for the preset switching on the FCBInfinity

      // If the stompBox button is pressed, we want to switch to STOMP_MODE_SCENE
      if (btnStompMode.fallingEdge()) {
        // Switch to looper mode
        setStompBoxMode(STOMP_MODE_SCENE);
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
          AxeMidi.sendPresetChange(g_iCurrentPreset);
          return;
        }
      }

      // The bottom 4 buttons indicate which preset is selected
      // lets calculate which light to turn on
      int light = constrain((g_iCurrentPreset-1)%4, 0, 3);
      // Loop through the lower row of buttons and enable or disable the leds
      for(int i=0; i<4; ++i)
        btnRowLower[i].setLed(light==i);

      // If the bankUp/Down buttons are pressed update the presetBank variable.
      // Later on we might want to flash preset switching buttons or something and have a feature where you
      // cycle through banks faster by keeping the button pressed
      // For now, KISS :P
      if (btnBankUp.fallingEdge()) {
        ++g_iPresetBank;
        g_iPresetBank = constrain(g_iPresetBank, 0, 999);
        setLedDigitValue(g_iPresetBank);
        return;
      }
      if (btnBankDown.fallingEdge()) {
        --g_iPresetBank;
        g_iPresetBank = constrain(g_iPresetBank, 0, 999);
        setLedDigitValue(g_iPresetBank);
        return;
      }

      // Right, the top buttons command a few effect blocks, namely:
      // Drive1, Delay1, Chorus1, Flanger1, Pitch1/Wah1 swapper
      // The latter one has Wah1 ON and Pitch1 OFF in unstomped state, and toggles the states when stomped.
      // Is the button pressed, lets toggle. If it's not set, lets update the led to the same state.
      if (handleEffectStompButton(&btnRowUpper[0], AXEFX_EFFECTID_Drive1,   AXEFX_EFFECTID_Drive2)) return;
      if (handleEffectStompButton(&btnRowUpper[1], AXEFX_EFFECTID_Delay1,   AXEFX_EFFECTID_Delay2,    AXEFX_EFFECTID_Multidelay1)) return;
      if (handleEffectStompButton(&btnRowUpper[2], AXEFX_EFFECTID_Chorus1,  AXEFX_EFFECTID_Chorus2)) return;
      if (handleEffectStompButton(&btnRowUpper[3], AXEFX_EFFECTID_Flanger1, AXEFX_EFFECTID_Flanger2)) return;

      // This is the tricky one because it controls two effects
      // In off state, WAH1 will be controlled by expPedal1ToeSwitch, in on state
      // Pitch1 will be controlled. This way I can switch between whammy and wah
      // with one stomp.
      handleWahPitchToggle(&btnRowUpper[4]);

    } // end of STOMP_MODE_NORMAL
    break;

    case STOMP_MODE_10STOMPS: {
      // In this mode the RGB-led is colored blue and all the upper and lower buttons
      // toggle effect bypasses, the BankUp/Down now just select the next or previous
      // preset.

      // If the stompBox button is pressed, we want to switch to STOMP_MODE_SCENE
      if (btnStompMode.fallingEdge()) {
        // Switch to looper mode
        setStompBoxMode(STOMP_MODE_SCENE);
        return;
      }

      // Same as the normal stomp mode, the top row will control the same effects
      if (handleEffectStompButton(&btnRowUpper[0], AXEFX_EFFECTID_Drive1,       AXEFX_EFFECTID_Drive2)) return;
      if (handleEffectStompButton(&btnRowUpper[1], AXEFX_EFFECTID_Delay1,       AXEFX_EFFECTID_Delay2,    AXEFX_EFFECTID_Multidelay2)) return;
      if (handleEffectStompButton(&btnRowUpper[2], AXEFX_EFFECTID_Chorus1,      AXEFX_EFFECTID_Chorus2)) return;
      if (handleEffectStompButton(&btnRowUpper[3], AXEFX_EFFECTID_Flanger1,     AXEFX_EFFECTID_Flanger2)) return;

      // Handle the WAH/Pitch toggle
      handleWahPitchToggle(&btnRowUpper[4]);

      // The bottom row will have four extra effect toggles now:
      // Rev1, Multidelay1, Phaser1 and Rotary1
      if (handleEffectStompButton(&btnRowLower[0], AXEFX_EFFECTID_Reverb1,      AXEFX_EFFECTID_Reverb2)) return;
      if (handleEffectStompButton(&btnRowLower[1], AXEFX_EFFECTID_Multidelay1,  AXEFX_EFFECTID_Multidelay2,   AXEFX_EFFECTID_Delay2)) return;
      if (handleEffectStompButton(&btnRowLower[2], AXEFX_EFFECTID_Phaser1,      AXEFX_EFFECTID_Phaser2)) return;
      if (handleEffectStompButton(&btnRowLower[3], AXEFX_EFFECTID_Rotary1,      AXEFX_EFFECTID_Rotary2)) return;

      // In this mode the bankUp/Down buttons react a little different; instead of changing banks
      // they will just move to the next or previous preset on the AxeFx. We'll calculate the new
      // bank automatically when we receive the preset changed SysEx from the AxeFx.
      if (btnBankUp.fallingEdge()) {
        AxeMidi.sendPresetChange(g_iCurrentPreset+1);
        return;
      }
      if (btnBankDown.fallingEdge()) {
        AxeMidi.sendPresetChange(g_iCurrentPreset-1);
        return;
      }

    } // end of STOMP_MODE_10STOMPS
    break;

    case STOMP_MODE_SCENE: {
      // In this mode the RGB-led is colored blue and all the upper and lower buttons
      // toggle effect bypasses, the BankUp/Down now just select the next or previous
      // preset.

      // If the stompBox button is pressed, we want to switch to STOMP_MODE_LOOPER
      if (btnStompMode.fallingEdge()) {
        // Switch to looper mode
        setStompBoxMode(STOMP_MODE_LOOPER);
        return;
      }

      // Same as the normal stomp mode, the top row will control the same effects
      if (handleEffectStompButton(&btnRowUpper[0], AXEFX_EFFECTID_Multidelay1,  AXEFX_EFFECTID_Multidelay2)) return;
      if (handleEffectStompButton(&btnRowUpper[1], AXEFX_EFFECTID_Delay1,       AXEFX_EFFECTID_Delay2,    AXEFX_EFFECTID_Multidelay2)) return;
      if (handleEffectStompButton(&btnRowUpper[2], AXEFX_EFFECTID_Chorus1,      AXEFX_EFFECTID_Chorus2)) return;
      if (handleEffectStompButton(&btnRowUpper[3], AXEFX_EFFECTID_Flanger1,     AXEFX_EFFECTID_Flanger2)) return;

      // Handle the WAH/Pitch toggle
      handleWahPitchToggle(&btnRowUpper[4]);

      // Loop through all the bottom buttons and set the led status
      // according to the current scene. If the button is pressed set
      // it to the new scene
      for (int i=0; i<5; ++i) {
        btnRowLower[i].setLed(g_iCurrentScene == i);

        if (btnRowLower[i].btn.fallingEdge()) {
          setScene(i);
          return;
        }
      }

      // In this mode the bankUp/Down buttons react a little different; instead of changing banks
      // they will just move to the next or previous preset on the AxeFx. We'll calculate the new
      // bank automatically when we receive the preset changed SysEx from the AxeFx.
      if (btnBankUp.fallingEdge()) {
        AxeMidi.sendPresetChange(g_iCurrentPreset+1);
        return;
      }
      if (btnBankDown.fallingEdge()) {
        AxeMidi.sendPresetChange(g_iCurrentPreset-1);
        return;
      }

    } // end of STOMP_MODE_SCENE
    break;

    case STOMP_MODE_LOOPER: {
      // This mode changes the top buttons into the controls for the looper in the AxeFx

      // If the mode button is pressed, switch back to the previous mode.
      if (btnStompMode.fallingEdge()) {
        // Restore the old mode
        AxeMidi.requestLooperUpdates(false);
        setStompBoxMode(l_iMainStompBoxMode);
        return;
      }

      // see FCBEffectManager.h for the various CC commands
      // The bottom row controls the looper record, play, once, dub and x/y
      if (btnRowLower[0].btn.fallingEdge())
        FCBLooperEffect.setPlay(!FCBLooperEffect.getPlay());
      btnRowLower[0].setLed(FCBLooperEffect.getPlay());

      if (btnRowLower[1].btn.fallingEdge())
        FCBLooperEffect.setRecord(!FCBLooperEffect.getRecord());
      btnRowLower[1].setLed(FCBLooperEffect.getRecord());

      if (btnRowLower[2].btn.fallingEdge())
        FCBLooperEffect.setOnce(!FCBLooperEffect.getOnce());
      btnRowLower[2].setLed(FCBLooperEffect.getOnce());

      if (btnRowLower[3].btn.fallingEdge())
        FCBLooperEffect.setDub(!FCBLooperEffect.getDub());
      btnRowLower[3].setLed(FCBLooperEffect.getDub());


      // The top row controls the looper reverse, Half-speed, Undo, Metronome, bypass.
      if (btnRowUpper[0].btn.fallingEdge())
        FCBLooperEffect.setReverse(!FCBLooperEffect.getReverse());
      btnRowUpper[0].setLed(FCBLooperEffect.getReverse());

      if (btnRowUpper[1].btn.fallingEdge())
        FCBLooperEffect.setHalfSpeed(!FCBLooperEffect.getHalfSpeed());
      btnRowUpper[1].setLed(FCBLooperEffect.getHalfSpeed());

      if (btnRowUpper[2].btn.fallingEdge())
        FCBLooperEffect.setUndo(!FCBLooperEffect.getUndo());
      btnRowUpper[2].setLed(FCBLooperEffect.getUndo());

      if (btnRowUpper[3].btn.fallingEdge())
        FCBLooperEffect.setMetronome(!FCBLooperEffect.getMetronome());
      btnRowUpper[3].setLed(FCBLooperEffect.getMetronome());

      if (btnRowUpper[4].btn.fallingEdge())
        FCBLooperEffect.setBypass(!FCBLooperEffect.getBypass());
      btnRowUpper[4].setLed(FCBLooperEffect.getBypass());

      // In this mode the bankUp/Down buttons react a little different; instead of changing banks
      // they will just move to the next or previous preset on the AxeFx. We'll calculate the new
      // bank automatically when we receive the preset changed SysEx from the AxeFx.
      if (btnBankUp.fallingEdge()) {
        AxeMidi.sendPresetChange(g_iCurrentPreset+1);
        return;
      }
      if (btnBankDown.fallingEdge()) {
        AxeMidi.sendPresetChange(g_iCurrentPreset-1);
        return;
      }

    } // end of STOMP_MODE_LOOPER
    break;

  } // end of stompBoxMode switch


  // Play around with the expression pedals a little, Send CC# External_Control_1 on channel 1
  // Moved this below the switch, so we can override the functionality of the expressionPedals
  // in the various different modes, if we wish.
  if (ExpPedal1.hasChanged()) {
    //Serial.println(ExpPedal1.getRawValue());
    AxeMidi.sendControlChange(AXEFX_DEFAULTCC_Input_Volume, ExpPedal1.getValue());
  }

  // ExpPedal2 has a new value? Send CC# Input_Volume
  if (ExpPedal2.hasChanged()) {
    //Serial.println(ExpPedal2.getRawValue());
    // If the pitchshifter is enabled, send control2 messages, otherwise, just send control1
    if (FCBEffectManager[AXEFX_EFFECTID_Pitch1]->isActive())
      AxeMidi.sendControlChange(AXEFX_DEFAULTCC_External_Control_2, ExpPedal2.getValue());
    else
      AxeMidi.sendControlChange(AXEFX_DEFAULTCC_External_Control_1, ExpPedal2.getValue());
  }

  // Lower button 4 always controls the XY mode, this assumes there is always a AMP block in your
  // preset, the current XY state is copied off that Amp1 block and toggles all the other effects
  if (g_iStompBoxMode != STOMP_MODE_SCENE) {
    if (btnRowLower[4].btn.fallingEdge()) {
      FCBEffectManager[AXEFX_EFFECTID_Amp1]->setY(!FCBEffectManager[AXEFX_EFFECTID_Amp1]->isXMode(), AXEFX_DEFAULTCC_Amp_1_XY);
    }
    btnRowLower[4].setLed(!FCBEffectManager[AXEFX_EFFECTID_Amp1]->isXMode());
  }

  // Button under exppedal1 enables or disables WAH or Pitch1
  if (btnExpPedalRight.fallingEdge()) {
    // @TODO: Implement this
    Serial.println("TIPTOEBUTTON!!!");
    //AxeMidi.sendControlChange(AXEFX_DEFAULTCC_External_Control_2, 127);
  }

  // Button on top controls tuner and taptempo
  if (btnTapTempoTuner.read()==LOW && btnTapTempoTuner.duration()>1500) {
    // Send Tuner Request
    AxeMidi.sendControlChange(AXEFX_DEFAULTCC_Tuner, 127);
  }
  else if (btnTapTempoTuner.fallingEdge()) {
    if (AxeMidi.isTunerOn()) {
      // Turn tuner off if it's on
      AxeMidi.sendControlChange(AXEFX_DEFAULTCC_Tuner, 0);
    } else {
      // Tuner is off, send TapTempo CC
      AxeMidi.sendControlChange(AXEFX_DEFAULTCC_Tempo, 127);
    }
  }

  // Lets check if we received a MIDI message
  // Or lets not, since we're not doing anything with incoming -non sysex-
  // midi messages at the moment. But lets leave this in in case someone else
  // does want to do something with incoming midi messages.
  // The midi class will call our onAxeFxSysExMessage() callback when we receive
  // an incoming SysEx from the AxeFx
  /*
  if (AxeMidi.hasMessage()) {
    // We've got an incoming midi message, see AxeMidi.h and io_MIDI.h for more info
    // on message types and more info. You could also use the callback features in
    // the Midi class to separate this logic.
    // AxeMidi.getType()
    // AxeMidi.getData1()
    // AxeMidi.getData2();
    // AxeMidi.getSysExArray();
  }
  */

} // loop()

// End of FCBInfinity.ino