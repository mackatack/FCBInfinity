#include <Wprogram.h>
#include <MIDI.h>
#include "io_AxeMidi.h"
#include "fcbinfinity.h"

#include "MIDI.h"
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "HardwareSerial.h"
#define Channel_Refused 0

/*! to ATmega 644p users: this library uses the serial port #1 for MIDI. */
#if defined(__AVR_ATmega644P__)
#undef HWSerial
#define HWSerial Serial1
/*! to Teensy/Teensy++ users: this library uses the Tx & Rx pins (the USB is not used) */
#elif defined(CORE_TEENSY)
#undef HWSerial
HardwareSerial HWSerial = HardwareSerial();
#else
/*! to any other ATmega users: this library uses the serial port #0 for MIDI (if you have more than one HWSerial).*/
#undef HWSerial
#define HWSerial Serial
#endif

/* Main instance the class comes pre-instantiated, just like the Midi class does. */
AxeMidi_Class AxeMidi;

/**
 * Constructor
 */
AxeMidi_Class::AxeMidi_Class() {
  m_bSendReceiveChecksummedSysEx = false;
}

// Initialize the static constant that holds the note names
const char *AxeMidi_Class::notes[] = {
  "A ", "A#", "B ", "C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#"
};

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
  HWSerial.write(0xF0);
	HWSerial.write(array, length);

  // More info on checksumming see:
  // http://wiki.fractalaudio.com/axefx2/index.php?title=MIDI_SysEx
  if (m_bSendReceiveChecksummedSysEx) {
    int sum = 0xF0;
    for (int i=0; i<length; ++i)
      sum = sum ^ (int)array[i];
    HWSerial.write(sum & 0x7F);
  }

	HWSerial.write(0xF7);
}

/**
 * Tell the AxeFx to send the PresetName over Midi
 */
byte msgRequestPresetName[] = {0x00, 0x01, 0x74, 0x01, 0x0f};
void AxeMidi_Class::requestPresetName() {
  sendSysEx(5, msgRequestPresetName);
}

/**
 * Tell the AxeFx to start the tuner and send us the realtime
 * midi messages
 *
 * @TODO implement this, for now just start the tuner manually on the AxeFx
 */
void AxeMidi_Class::startTuner() {
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
