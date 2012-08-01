/**
 * version 1.0 by mackatack@gmail.com
 * Released into the public domain.
 *
 * Use at own risk.
 */

#ifndef FCBInfinity_SettingsH
#define FCBInfinity_SettingsH

#include <Wprogram.h>

class FCBSettings_Class {
  public:
    FCBSettings_Class();

    // Settings for the expression pedals
    #define SETTINGS_EXPPEDAL_ADDRESS 50
    #define SETTINGS_EXPPEDAL_STORE_COUNT 10
    int getExpPedalCalibrationLow(int expPedalNum);
    int getExpPedalCalibrationHigh(int expPedalNum);
    void setExpPedalCalibration(int expPedalNum, int calibrationLow, int calibrationHigh);

    // These functions read and write 2byte values to the EEPROM
    int read2ByteInt(int address);
    void write2ByteInt(int address, int value);
};

extern FCBSettings_Class FCBSettings;

#endif
