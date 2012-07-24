/**
 * AxeMidi - An extension of the Midi class originally created by Francois Best.
 * It adds some functions that can not be handled directly by the original Midi class, such as checksumming
 * of sysex messages etc.
 * version 1.0 by mackatack@gmail.com
 * Released into the public domain.
 *
 * Note: this has only been tested in combination with the Midi_Class object that's bundled
 * with the Arduino SDK. Use at own risk.
 */

#ifndef AxeMidi_Class_H
#define AxeMidi_Class_H

#include <Wprogram.h>
#include <MIDI.h>

class AxeMidi_Class: public MIDI_Class {
  public:
    void startTuner();
    void readTuner();
    void sendCheckedSysex();
    void finalizeCheckedSysex();
};

extern AxeMidi_Class AxeMidi;

#endif
