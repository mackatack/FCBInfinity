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

//

class AxeMidi_Class: public MIDI_Class {
  public:
    void startTuner();
    void readTuner();
    void sendCheckedSysex();
    void finalizeCheckedSysex();
    
    /* Check for new incoming MIDI messages, call this once every loop cycle */
    boolean handleMidi();
    /* hasMessage returns whether or not we received a message this loop */
    boolean hasMessage();
   
    /*! Receive a Note On message */
    void onNoteOn(byte NoteNumber,byte Velocity,byte Channel);
    /*! Receive a Note Off message (a real Note Off, not a Note On with null velocity) */
    void onNoteOff(byte NoteNumber,byte Velocity,byte Channel);
    /*! Receive a Program Change message */
    void onProgramChange(byte ProgramNumber,byte Channel);
    /*! Receive a Control Change message */
    void onControlChange(byte ControlNumber, byte ControlValue,byte Channel);
    /*! Receive AfterTouch (carries the information of pressure of the given key/note) */
    void onPolyPressure(byte NoteNumber,byte Pressure,byte Channel);
    /*! Receive AfterTouch */
    void onAfterTouch(byte Pressure,byte Channel);

    void onRawSysex();
    void onTunerData();
    void onPatchInfo();
  private:
    boolean m_bHasMessage;
};

extern AxeMidi_Class AxeMidi;

#endif
