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

