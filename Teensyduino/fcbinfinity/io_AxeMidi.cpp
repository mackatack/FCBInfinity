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
 * This function tells the AxeFx to switch to a new preset
 * @TODO calculate the AxeFx bank and send the correct CC#0 value
 * if iAxeFxPresetNumber > 127
 */
void AxeMidi_Class::sendPresetChange(int iAxeFxPresetNumber, int iChannel) {
  // CC 0: 0 sets AxeFx bank to A, it might be a solution for the
  // problem I had when sending PC#2 and the AxeFx jumping to #384 (Bypass)
  // I'm not quite sure we're required to send this every PC, but we'll test
  // that later.
  // Thanks to Slickstring/Reincaster for the hint! :P

  // After some testing it seemed that AxeFx > IO > Midi > Mapping Mode was set to
  // Custom, setting back to None fixed the above problem.

  // It seems the Axe wont do subsequent preset changes
  // unless we send it some other midi message. Lets just keep this
  // bank mode switcher code in place for now
  AxeMidi.sendControlChange(0, 0, iChannel);

  // Send the PC message
  AxeMidi.sendProgramChange(iAxeFxPresetNumber-1, iChannel);
}

/**
 * Tells the AxeFx to switch to either the X or Y mode for
 * all it's effects.
 * See the link below for more info about the CC numbers
 * http://wiki.fractalaudio.com/axefx2/index.php?title=MIDI_CCs_list
 */
void AxeMidi_Class::sendToggleXY(boolean bYModeOn, int iChannel) {
  // CC 100 to 119 control all the x/y for all the effects, just toggle them all.
  // If bYModeOn is true, send 127, otherwise send 0
  for (int cc=100; cc<=119; ++cc)
    AxeMidi.sendControlChange(cc, bYModeOn?127:0, iChannel);
}

/**
 * Overrides the sendSysEx in MIDI.cpp.
 * Just send a sysex message via MIDI, but track the checksum in the sysexChecksum variable
 * if m_bSendReceiveChecksummedSysEx is true
 */
void AxeMidi_Class::sendSysEx(byte length, byte * sysexData) {
  HWSerial.write(0xF0);
  HWSerial.write(sysexData, length);

  // More info on checksumming see:
  // http://wiki.fractalaudio.com/axefx2/index.php?title=MIDI_SysEx
  if (m_bSendReceiveChecksummedSysEx) {
    byte sum = 0xF0;
    for (int i=0; i<length; ++i)
      sum = sum ^ sysexData[i];
    HWSerial.write(sum & 0x7F);
  }

  HWSerial.write(0xF7);
}

/**
 * This just sends a bogus message to test if the AxeFx is echoing our messages back
 * if so, the user should disable thru...
 */
void AxeMidi_Class::sendLoopbackCheck() {
  byte msgRequestPresetName[] = {
    SYSEX_LOOBACK_CHECK_DATA,
    SYSEX_LOOBACK_CHECK_DATA,
    SYSEX_LOOBACK_CHECK_DATA,
    SYSEX_LOOBACK_CHECK_DATA,
    SYSEX_LOOBACK_CHECK_DATA
  };
  sendSysEx(5, msgRequestPresetName);
}

/**
 * Tell the AxeFx to send the PresetName over Midi
 */
void AxeMidi_Class::requestPresetName() {
  static const byte msgRequestPresetName[] = {
    SYSEX_AXEFX_MANUFACTURER,
    SYSEX_AXEFX_MODEL,
    SYSEX_AXEFX_PRESET_NAME
  };
  sendSysEx(5, (byte*)msgRequestPresetName);
}


/**
 * Tell the AxeFx to send the bypass states for the current preset's effects
 * http://forum.fractalaudio.com/other-midi-controllers/39161-using-sysex-recall-present-effect-bypass-status-info-available.html
 */

void AxeMidi_Class::requestBypassStates() {
  static const byte msgRequestBypassStates[] = {
    SYSEX_AXEFX_MANUFACTURER,
    SYSEX_AXEFX_MODEL,
    SYSEX_AXEFX_GET_PRESET_EFFECT_BLOCKS_AND_CC_AND_BYPASS_STATE
  };
  sendSysEx(5, (byte*)msgRequestBypassStates);
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
