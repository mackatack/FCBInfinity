/**
 * ExpPedals, this class separates all the logic required for reading the expression pedals
 * version 1.0 by mackatack@gmail.com
 * Released into the public domain.
 *
 * Use at own risk.
 */

#ifndef FCBInfinity_ExpPedalsH
#define FCBInfinity_ExpPedalsH

#include <Wprogram.h>

class ExpPedals_Class {
  public:
    ExpPedals_Class(int pin);
    boolean update();
    boolean hasChanged();
    int getValue();
    int getRawValue();

    int getPin();
    void setPin(int pin);

    void setCalibration(int low, int high);
    int getCalibrationLow();
    int getCalibrationHigh();
  private:
    int m_iPin;

    int m_iPrevValue1;
    int m_iPrevValue2;
    int m_iRawValue;
    int m_iValue;
    boolean m_bChanged;

    int m_iLowCalibration;
    int m_iHighCalibration;
};


// Make the expressionpedals available globally,
// if you need more just uncomment and check the .cpp file as well
extern ExpPedals_Class ExpPedal1;
extern ExpPedals_Class ExpPedal2;
//extern ExpPedals_Class ExpPedal3;
//extern ExpPedals_Class ExpPedal4;
//extern ExpPedals_Class ExpPedal5;
//extern ExpPedals_Class ExpPedal6;

#endif
