#include <Wprogram.h>
#include "io_ExpPedals.h"

/**
 * Constructor
 */
ExpPedals_Class::ExpPedals_Class(int pin) {
  m_iPin = pin;
  m_iPrevValue1 = 0;
  m_iPrevValue2 = 0;
  m_iValue = 0;
  m_bChanged = false;
  m_iLowCalibration = 0;
  m_iHighCalibration = 1024;
}

/**
 * Checks the analog pin for a changed value. This library keeps the last few values
 * to even out sudden changes in a attempt to get more reliable values.
 */
boolean ExpPedals_Class::update() {
  m_iRawValue = analogRead(m_iPin);

  // Map the raw value between 0 and 1024 or the set calibration values
  // and calculate the corresponding midi range between 0 and 127
  int v = map(m_iRawValue, m_iLowCalibration, m_iHighCalibration, 0, 127);

  // Early check, if theres no change in the unsmoothed value, just return
  // no need to do all the stuff below.
  if (v == m_iValue) {
    m_bChanged = false;
    return false;
  }

  // The new value is different, try to smooth out the new values
  // by calculating the average value over the last 3 values.
  v = (m_iPrevValue2 + m_iPrevValue1 + m_iValue + v)/4;

  // Shift the previous values
  m_iPrevValue2 = m_iPrevValue1;
  m_iPrevValue1 = m_iValue;

  // Now check if the calculated average is different
  if (v == m_iValue) {
    // Nope, the avg value is still the same, no change.
    m_bChanged = false;
    return false;
  }

  // Yes, the new average value is different, update the variables
  // and return true.
  m_iValue = v;
  m_bChanged = true;
  return true;
}

/**
 * This lets you know if the value has changed since the last update()
 */
boolean ExpPedals_Class::hasChanged() {
  return m_bChanged;
}

/**
 * This sets the state of the expression back to unchanged. This is handy
 * in case you've handled the expressionPedal value and you dont want anything
 * else to use the updated value after your statement.
 */
void ExpPedals_Class::setHandled() {
  m_bChanged = false;
}

/**
 * Get the Midi value of this pedal (0 to 127)
 */
int ExpPedals_Class::getValue() {
  return m_iValue;
}

/**
 * Get the RAW value of this pedal (0 to 1024)
 */
int ExpPedals_Class::getRawValue() {
  return m_iRawValue;
}

/**
 * Get and set the pin this ExpPedal is connected to.
 */
int ExpPedals_Class::getPin() {
  return m_iPin;
}
void ExpPedals_Class::setPin(int pin) {
  m_iPin = pin;
}

/**
 * These functions should allow one to make a menu and
 * provide a way to calibrate the pedals
 */
int ExpPedals_Class::getCalibrationLow() {
  return m_iLowCalibration;
}
int ExpPedals_Class::getCalibrationHigh() {
  return m_iHighCalibration;
}
void ExpPedals_Class::setCalibration(int low, int high) {
  m_iHighCalibration = high;
  m_iLowCalibration = low;
}
