#include <Wprogram.h>
#include "ExpPedals.h"
#include "fcbinfinity.h"

/* Main instance the class comes pre-instantiated */
ExpPedals_Class ExpPedal1(A6);  // Analog pin 6 (pin 44)
ExpPedals_Class ExpPedal2(A7);  // Analog pin 7 (pin 45)
//ExpPedals_Class ExpPedal3(2);
//ExpPedals_Class ExpPedal4(2);
//ExpPedals_Class ExpPedal5(2);
//ExpPedals_Class ExpPedal6(2);

boolean ExpPedals_Class::update() {
  //delay(5);
  //analogRead(m_iPin);
  //delay(5);

  ledControl.setDigit(0, 0, 8, true);

  m_iRawValue = analogRead(m_iPin);
  int v = map(m_iRawValue, m_iLowCalibration, m_iHighCalibration, 0, 127);

  // Early check, if theres no change in the unsmoothed value, just return
  if (v == m_iValue) {
    m_bChanged = false;
    return false;
  }

  v = (m_iPrevValue2 + m_iPrevValue1 + m_iValue + v)/4;

  m_iPrevValue2 = m_iPrevValue1;
  m_iPrevValue1 = m_iValue;

  if (v == m_iValue) {
    m_bChanged = false;
    return false;
  }

  m_iValue = v;
  m_bChanged = true;
  return true;
}
boolean ExpPedals_Class::hasChanged() {
  return m_bChanged;
}
int ExpPedals_Class::getValue() {
  return m_iValue;
}
int ExpPedals_Class::getRawValue() {
  return m_iRawValue;
}


ExpPedals_Class::ExpPedals_Class(int pin) {
  m_iPin = pin;
  m_iPrevValue1 = 0;
  m_iPrevValue2 = 0;
  m_iValue = 0;
  m_bChanged = false;
  m_iLowCalibration = 0;
  m_iHighCalibration = 1024;
}

int ExpPedals_Class::getPin() {
  return m_iPin;
}
void ExpPedals_Class::setPin(int pin) {
  m_iPin = pin;
}
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
