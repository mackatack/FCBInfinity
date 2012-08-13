/**
 * Lets use some C++ object oriented sweetness to control what
 * state our FCBInfinity is in. A FCBMode controls how the FCBinfinity
 * will react to input. You can create your own mode from the template
 * and overload a few functions to override some functions.
 * This default mode just defines all the basic interaction, such as input
 * from buttons, expression pedals and midi messages. But also what will be
 * presented on the LCD and what leds are turned on and off.
 *
 * by mackatack@gmail.com
 * Released into the public domain.
 *
 * Use at own risk.
 */

#ifndef FCBInfinity_Mode_Default_H
#define FCBInfinity_Mode_Default_H

#include <Wprogram.h>

class FCBMode {
public:
  // Default constructor and destructor
  FCBMode();
  ~FCBMode();

  // Functions to handle the various inputs.
  void handleInputs();
  void handleMidi(byte type, byte data1, byte data2);
  void handleExpressionPedals();
  void handleStompModeButton();
  void handleStompButtons();
  void handleUpperStompButtons();
  void handleLowerStompButtons();
  void handleXYButton();
  void handleBankButtons();

  // These two get called by the AxeMidi callback, so handleInputs()
  // doesn't have to call them.
  void handleAxeFxSysex(byte * sysex, int length);
  void handleRawSysex(byte * sysex, int length);

  // This function gets called when the mode is activated for the
  // first time or when it's reactivated after another mode was active.
  // Handy for enabling timers or resetting initial states of indicator
  // leds.
  // @param A pointer to the last active mode, in case we want to restore
  // back to the previous mode.
  void restore(FCBMode * previousMode);
  void restoreLeds();
  void restoreUpperRowLeds();
  void restoreLowerRowLeds();
  void restoreXYLed();
  void restoreLedDigits();
  void restoreLCD();

  // This function gets called after it was active but now another mode
  // gets activated. It gives the mode a chance to clean-up some stuff;
  // disable timers and what not.
  // @param A pointer to the new mode, in case we want to override the change.
  void cleanup(FCBMode * newMode);

protected:
  // A pointer to the previously active mode
  FCBMode * m_pPreviousMode;
  bool m_isRestored;
};

#endif
