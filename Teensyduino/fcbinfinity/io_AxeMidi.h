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
#include "io_MIDI.h"

// A list of the currently available axe models
#define AXEMODEL_STANDARD 0
#define AXEMODEL_ULTRA    1
#define AXEMODEL_AXEFX2   3

#define AXE_MANUFACTURER_B1  0x00
#define AXE_MANUFACTURER_B2  0x01
#define AXE_MANUFACTURER_B3  0x74
#define AXE_MANUFACTURER  0x00, 0x01, 0x74

// The data we'll send to check the midiThru state
#define SYSEX_LOOBACK_CHECK_DATA 0x76

// The function id's we're using from the axe
#define SYSEX_AXEFX_FIRMWARE_VERSION 0x08
#define SYSEX_AXEFX_FIRMWARE_VERSION_AXE2 0x64
#define SYSEX_AXEFX_REALTIME_TUNER 0x0D
#define SYSEX_AXEFX_REALTIME_TEMPO 0x10
#define SYSEX_AXEFX_PRESET_NAME 0x0F
#define SYSEX_AXEFX_PRESET_CHANGE 0x14
#define SYSEX_AXEFX_GET_PRESET_EFFECT_BLOCKS_AND_CC_AND_BYPASS_STATE 0x0E
#define SYSEX_AXEFX_PRESET_MODIFIED 0x21

// Used to predefine the checksum byte in sysex messages
#define SYSEX_EMPTY_BYTE 0x00

class AxeMidi_Class {
  public:
    AxeMidi_Class();

    static const char *notes[]; //= {"A ", "A#", "B ", "C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#"};

    void startTuner();
    void requestPresetNumber();
    void requestPresetName();
    void requestBypassStates();
    void sendSysEx(byte length, byte * array);
    void sendPresetChange(int iAxeFxPresetNumber);
    void sendToggleXY(boolean bYModeOn);
    void sendControlChange(int cc, int value);
    void sendProgramChange(int pc);
    void sendLoopbackAndVersionCheck();

    boolean isInitialized() {
      return m_bFirmwareVersionReceived;
    }
    boolean isTunerOn() {
      return m_bTunerOn;
    }

    /* Check for new incoming MIDI messages, call this once every loop cycle */
    boolean handleMidi();
    /* hasMessage returns whether or not we received a message this loop */
    boolean hasMessage();

    /*
     * gets the AxeModel
     * AxeFX-II requires us to send checksummed SysEx messages over midi and
     * it will also send checksummed messages. This boolean controls whether or not
     * to send the extra byte checksum and allows older AxeFX models to also use this
     * library.
     */
    int getModel();

    /* sets the callback functions for incoming sysex messages */
    void registerAxeSysExReceiveCallback( void (*func)(byte*,int) );
    void registerRawSysExReceiveCallback( void (*func)(byte*,int) );

    void registerAxeFxConnectedCallback( void (*func)() );
    void registerAxeFxDisconnectedCallback( void (*func)() );

    /* gets and sets the midi channel this library will send messages on */
    void setMidiSendChannel(int channel) {m_iAxeChannel=channel;}
    int setMidiSendChannel() {return m_iAxeChannel;}

  protected:
    boolean m_bHasMessage;
    int m_iAxeModel;
    int m_iAxeChannel;
    boolean m_bFirmwareVersionReceived;
    boolean m_bTunerOn;

    /* Function pointer to a user defined function that handles raw sysex messages */
    void (*m_fpRawSysExCallback)(byte * sysex, int length);
    /* Function pointer to a user defined function that handles axefx sysex messages */
    void (*m_fpAxeFxSysExCallback)(byte * sysex, int length);

    /* Function pointer to a user defined function that handles the connection of an AxeFx */
    void (*m_fpAxeFxConnectedCallback)();
    void (*m_fpAxeFxDisconnectedCallback)();
};

extern AxeMidi_Class AxeMidi;

#endif
