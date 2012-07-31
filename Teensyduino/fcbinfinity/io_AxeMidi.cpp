#include <Wprogram.h>
#include <MIDI.h>
#include "io_AxeMidi.h"

/* Main instance the class comes pre-instantiated, just like the Midi class does. */
AxeMidi_Class AxeMidi;

AxeMidi_Class::AxeMidi_Class() {
  m_bSendReceiveChecksummedSysEx = false;
}


void startTuner() {
}

void readTuner() {
}

/**
 * Updates the library and checks for new midi messages, call this once
 * every loop, or only after all the modules have had a chance to process
 * the midi message or you might lose messages.
 */
boolean AxeMidi_Class::handleMidi() {

  if (!AxeMidi.read(MIDI_CHANNEL_OMNI)) {
    m_bHasMessage = false;
    return false;
  }

  m_bHasMessage = true;

  Serial.print("Type: ");
  Serial.print(AxeMidi.getType());
  Serial.print(", data1: ");
  Serial.print(AxeMidi.getData1());
  Serial.print(", data2: ");
  Serial.print(AxeMidi.getData2());
  Serial.print(", Channel: ");
  Serial.print(AxeMidi.getChannel());
  Serial.println(", MIDI OK!");

  return true;
}

/**
 * This returns whether or not we have received a midi message.
 */
boolean AxeMidi_Class::hasMessage() {
  return m_bHasMessage;
}

/**
 * Overrides the sendSysEx in MIDI.cpp.
 * Just send a sysex message via MIDI, but track the checksum in the sysexChecksum variable
 * if m_bSendReceiveChecksummedSysEx is true
 */
void AxeMidi_Class::sendSysEx(byte length, byte * array) {
  //UART.write(0xF0);
	//UART.write(array, length);

  // More info on checksumming see:
  // http://wiki.fractalaudio.com/axefx2/index.php?title=MIDI_SysEx
  if (m_bSendReceiveChecksummedSysEx) {
    byte sum = 0xF0;
    for (byte i=0; i<length; ++i)
      sum ^= array[i];
    //UART.write(sum && 0x7F);
  }

	//UART.write(0xF7);
}

/**
 *
 */
void AxeMidi_Class::requestPresetName() {
}

/**
 * The AxeFX-II requires us to send checksummed SysEx messages over midi and
 * it will also send checksummed messages. This boolean controls whether or not
 * to send the extra byte checksum and allows older AxeFX models to also use this
 * library. If you want to use this library with the AxeFX-II you should set this
 * variable to true after the initialization of the library.
 */
boolean AxeMidi_Class::getSendReceiveChecksummedSysEx() {
  return m_bSendReceiveChecksummedSysEx;
}
void AxeMidi_Class::setSendReceiveChecksummedSysEx(boolean sendReceiveChecksummedSysex) {
  m_bSendReceiveChecksummedSysEx = sendReceiveChecksummedSysex;
}
