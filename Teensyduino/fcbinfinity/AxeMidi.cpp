#include <Wprogram.h>
#include <MIDI.h>
#include "AxeMidi.h"

/* Main instance the class comes pre-instantiated, just like the Midi class does. */
AxeMidi_Class AxeMidi;

// A variable that tracks the checksum of sysex messages while we're sending.
byte sysexChecksum;

void startTuner() {
}

void readTuner() {
}

/**
 * Just send a sysex message via MIDI, but track the checksum in the sysexChecksum variable
 */
void sendCheckedSysex() {
}
/**
 * Send the sysex message checksum over MIDI and reset it to zero so we can send a new
 * sysex message immediately again.
 */
void finalizeCheckedSysex() {
}

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

boolean AxeMidi_Class::hasMessage() {
  return m_bHasMessage;
}
