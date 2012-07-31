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
    AxeMidi_Class();

    void startTuner();
    void readTuner();
    void sendSysEx(byte length, byte * array);

    /* Check for new incoming MIDI messages, call this once every loop cycle */
    boolean handleMidi();
    /* hasMessage returns whether or not we received a message this loop */
    boolean hasMessage();

    void requestPresetName();


    void onRawSysex();
    void onTunerData();
    void onPatchInfo();

    /**
     * The AxeFX-II requires us to send checksummed SysEx messages over midi and
     * it will also send checksummed messages. This boolean controls whether or not
     * to send the extra byte checksum and allows older AxeFX models to also use this
     * library. If you want to use this library with the AxeFX-II you should set this
     * variable to true after the initialization of the library.
     */
    boolean getSendReceiveChecksummedSysEx();
    void setSendReceiveChecksummedSysEx(boolean sendReceiveChecksummedSysex);

  protected:
    boolean m_bHasMessage;
    boolean m_bSendReceiveChecksummedSysEx;
};

extern AxeMidi_Class AxeMidi;

#endif
