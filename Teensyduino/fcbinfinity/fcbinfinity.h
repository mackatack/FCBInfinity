/**
 * FCBInfinity.h header file.
 * This file is used to create defines for objects and constants that should be
 * reusable the entire FCBInfinity project, such as access to the lcd, ledControl,
 * buttons and midi.
 *
 * Most of the objects declared below are initialized in fcbinfinity.ino.
 */

#ifndef FCBINFINITY_MAIN_H
#define FCBINFINITY_MAIN_H

  #include <Bounce.h>
  #include <LedControl.h>
  #include <LiquidCrystalFast.h>
  #include "io_MIDI.h"
  #include "io_AxeMidi.h"
  #include "io_ExpPedals.h"
  #include "modes_DefaultMode.h"

  // Pinouts for various IO on the teensy board.
  // See http://www.pjrc.com/teensy/teensyduino.html for more information about the pinout

  #define PIN_ONBOARD_LED 6
  #define PIN_RGBLED_R 27
  #define PIN_RGBLED_G 1
  #define PIN_RGBLED_B 0

  // This statement makes the ledControl object available throughout the entire
  // FCBInfinity source code. This object controls all the single color leds and the
  // led-digits on the fcbinfinity board. This does NOT control the RGB led, since we
  // want to control that using PWM ports on the teensy. The PWM pins allow us to vary
  // the intensity per channel.
  // More info about the LedControl library
  // http://www.pjrc.com/teensy/td_libs_LedControl.html
  //
  // constructor: LedControl(DIN_pin, CLK_pin, LOAD_pin, number_of_chips);
  extern LedControl ledControl;


  // This statement makes the lcd object available throughout the entire
  // FCBInfinity source code. This object controls the Liquid Crystal Display
  // that is used in this project.
  // More info about the LedControl library
  // http://www.pjrc.com/teensy/td_libs_LiquidCrystal.html
  //
  // constructor: LiquidCrystalFast lcd(RS, RW, Enable, D4, D5, D6, D7);
  extern LiquidCrystalFast lcd;


  // A struct that combines information for the original buttons on the FCB1010 that
  // have a led to indicate their status. Thus; the 1 - 10 buttons.
  typedef struct {
    int y;
    int x;
    Bounce btn;
    boolean ledStatus;
    void setLed(bool state) {
      ledStatus = state;
      ledControl.setLed(0, x, y, state);
    }
  } _FCBInfButton;

  // Make the buttons accessible throughout the entire project.
  extern _FCBInfButton btnRowUpper[];
  extern _FCBInfButton btnRowLower[];
  extern Bounce btnBankUp;
  extern Bounce btnBankDown;
  extern Bounce btnStompBank;


  // Make the expressionpedals available globally,
  // if you need more pedals just add here and initialize them in
  // the .ino file
  extern ExpPedals_Class ExpPedal1;
  extern ExpPedals_Class ExpPedal2;

  // Make the function to set modes public throughout the project
  extern void setCurrentMode(FCBMode* newMode);
  extern FCBMode* getCurrentMode();
  extern FCBMode* getPreviousMode();


  // A neat little debugging function that just dumps all bytes of a byte array to
  // the serial line
  inline void bytesHexDump(byte * bytes, int length) {
    Serial.print(" HEX:  ");
    for(int i=0; i<length; ++i) {
      Serial.print(bytes[i], HEX);
      Serial.print(" ");
    }
  };

  // Some functions to convert bytes to nibbles and back
  inline byte byteToLS(byte b) {
    return b & 0xf;
  }
  inline byte byteToMS(byte b) {
    return (b >> 4) & 0xf;
  }
  inline byte LSMSToByte(byte ls, byte ms) {
    return (ms & 0xf << 4) | ls & 0xf;
  }

#endif //FCBINFINITY_MAIN_H
