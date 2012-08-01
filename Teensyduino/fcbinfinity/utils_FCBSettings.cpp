#include <Wprogram.h>
#include "utils_FCBSettings.h"
#include <EEPROM.h>

FCBSettings_Class FCBSettings;



/**
 * The functions to get and set the calibration settings of the expression pedals
 * currenly enough memory on the EEPROM is reserved to handle 10 expression pedals
 * From SETTINGS_EXPPEDAL_ADDRESS we first use 2 bytes to indicate that the memory has
 * been initialised, from there we use four bytes per pedal to store the low and high values
 * for the calibration. So in total we use 42 bytes to store the calibration data,
 * or SETTINGS_EXPPEDAL_STORE_COUNT*2 + 2
 */

void initializeExpPedalMemory() {
  // Initialize the eeprom memory per pedal
  for(int pedal=0; pedal<SETTINGS_EXPPEDAL_STORE_COUNT; ++pedal) {
    FCBSettings.setExpPedalCalibration(pedal, 0, 1024);
  }

  // Write the initialization done indicator bytes
  //EEPROM.write(SETTINGS_EXPPEDAL_ADDRESS, 0xAC);
  //EEPROM.write(SETTINGS_EXPPEDAL_ADDRESS+1, 0xDC);
}
boolean checkExpPedalInitialised() {
  if (EEPROM.read(SETTINGS_EXPPEDAL_ADDRESS)==0xAC &&
      EEPROM.read(SETTINGS_EXPPEDAL_ADDRESS+1)==0xDC) return true; // AC/DC, tihihi :P

  // Hmm, memory hasn't been initialized, lets do so now.
  initializeExpPedalMemory();
}
int FCBSettings_Class::getExpPedalCalibrationLow(int expPedalNum) {
}
int FCBSettings_Class::getExpPedalCalibrationHigh(int expPedalNum) {
}
void FCBSettings_Class::setExpPedalCalibration(int expPedalNum, int calibrationLow, int calibrationHigh) {
  if (expPedalNum<0) return;
  if (expPedalNum>SETTINGS_EXPPEDAL_STORE_COUNT) return;
  if (calibrationLow<0) calibrationLow = 0;
  if (calibrationHigh>1024) calibrationHigh = 1024;
  if (calibrationHigh<calibrationLow) calibrationHigh = calibrationLow;
  write2ByteInt(SETTINGS_EXPPEDAL_ADDRESS+2 + expPedalNum*2, calibrationLow);
  write2ByteInt(SETTINGS_EXPPEDAL_ADDRESS+2 + expPedalNum*2 + 2, calibrationHigh);
}


int FCBSettings_Class::read2ByteInt(int address) {
}
void FCBSettings_Class::write2ByteInt(int address, int value){
}


FCBSettings_Class::FCBSettings_Class() {

  // Lets see if the memory for the expressionpedals has been initialized.
  checkExpPedalInitialised();
}


