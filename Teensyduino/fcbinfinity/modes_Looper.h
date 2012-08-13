/**
 * The only way the 10 stomps mode is different from the 'normal'
 * is that now the lower buttons all control an effect. Also, the
 * bank up and down buttons now move to a new bank.
 *
 * Doesn't the function override list look cute? :P It's really easy
 * to just override some small features while keeping everything else
 * reacting the same. Or you could just override everything :P
 *
 * mackatack@gmail.com
 * Released into the public domain.
 *
 * Use at own risk.
 */

#ifndef FCBInfinity_Mode_StompX10_H
#define FCBInfinity_Mode_StompX10_H

#include <Wprogram.h>

// Lets create a new mode and inherit everything from the default FCBMode
class FCBModeStompsX10: public FCBMode {
public:
  // This mode only changes how the lower buttons react
  void handleLowerStompButtons();

  // The bankbuttons now move to the next or previous preset
  void handleBankButtons();
};

extern FCBModeStompsX10 FCBModeStompsX10;

#endif
