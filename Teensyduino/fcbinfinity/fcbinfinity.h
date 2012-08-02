/**
 * FCBInfinity.h header file.
 * This file is used to create defines for objects and constants that should be
 * reusable the entire FCBInfinity project, such as access to the lcd, ledControl,
 * buttons and midi.
 *
 * Most of the objects declared below are initialized in fcbinfinity.ino.
 */

#ifndef FCBInfinity_Main_H
#define FCBInfinity_Main_H

  #include <Bounce.h>
  #include <LedControl.h>
  #include <LiquidCrystalFast.h>
  #include <MIDI.h>
  #include "io_AxeMidi.h"
  #include "io_ExpPedals.h"

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


#endif
