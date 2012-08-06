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
  m_iAxeModel = 3;
  m_bFirmwareVersionReceived;
  m_fpRawSysExCallback = NULL;
  m_fpAxeFxSysExCallback = NULL;
  //sendLoopbackCheck();
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

  // We've got a message!
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

  // Lets see if the message is a sysex and call the appropriate callbacks
  if (getType() == SysEx) {
    // Get the byte array SysEx message and the length of the message
    byte * sysex = AxeMidi.getSysExArray();
    int length = AxeMidi.getData1();

    if (sysex[5] == SYSEX_LOOBACK_CHECK_DATA) {
      // Oof, midi thru is enabled on the AxeFx, send a message to the user that they
      // need to disable it.
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("#WARNING# Disable   Midi Thru on AxeFx");
      delay(5000);
      lcd.clear();
      lcd.setCursor(0,0);
    }

    // In case the length > 4 and the AXE_MANUFACTURER bytes match, this
    // sysex is coming from the AxeFx unit. Lets do some basic handling and
    // if there are any callbacks registered, call them now
    if (length>4 &&
        sysex[1] == AXE_MANUFACTURER_B1 &&
        sysex[2] == AXE_MANUFACTURER_B2 &&
        sysex[3] == AXE_MANUFACTURER_B3) {

      // In case it's a firmware version response, lets
      // store the correct model info
      if (sysex[5] == SYSEX_AXEFX_FIRMWARE_VERSION ||
          sysex[5] == SYSEX_AXEFX_FIRMWARE_VERSION_AXE2) {
        Serial.println("RECEIVED FIRMWARE VERSION! <3");
        m_bFirmwareVersionReceived=true;
        m_iAxeModel = sysex[4];
      }

      // Lets see if we have a valid callback function for AxeFx sysex messages
      if (m_fpAxeFxSysExCallback != NULL)
        (m_fpAxeFxSysExCallback)(sysex, length);
    }
    else {
      // Not an axefx sysex, call the rawsysex callback
      if (m_fpRawSysExCallback != NULL)
        (m_fpRawSysExCallback)(sysex, length);
    }

    // Some debugging code to just dump the sysex data on the serial line.
    Serial.print("Sysex ");
    bytesHexDump(sysex, length);
    Serial.println(" ");
  }

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
 * if m_iAxeModel>=3
 */
void AxeMidi_Class::sendSysEx(byte length, byte * sysexData) {
  if (!m_bFirmwareVersionReceived &&
      sysexData[4] != SYSEX_AXEFX_FIRMWARE_VERSION &&
      sysexData[4] != SYSEX_LOOBACK_CHECK_DATA) {
    // If we don't have the firmware version yet, please request it from the
    // axefx, but dont send it if this is the actual firmware request
    sendLoopbackAndVersionCheck();
  }

  HWSerial.write(0xF0);
  HWSerial.write(sysexData, length);

  // More info on checksumming see:
  // http://wiki.fractalaudio.com/axefx2/index.php?title=MIDI_SysEx
  if (m_iAxeModel>=3) {
    byte sum = 0xF0;
    for (int i=0; i<length; ++i)
      sum = sum ^ sysexData[i];
    HWSerial.write(sum & 0x7F);
    Serial.print("Sending checksummed sysex: ");
    bytesHexDump(sysexData, length);
    Serial.println();
  } else {
    Serial.print("Sending unchecksummed sysex: ");
    bytesHexDump(sysexData, length);
    Serial.println();
  }

  HWSerial.write(0xF7);
}

/**
 * This just sends a bogus message to test if the AxeFx is echoing our messages back
 * if so, the user should disable thru...
 */
void AxeMidi_Class::sendLoopbackAndVersionCheck() {
  // Send the bogus loopback check data
  byte msgBogusLoopbackData[] = {
    SYSEX_LOOBACK_CHECK_DATA,
    SYSEX_LOOBACK_CHECK_DATA,
    SYSEX_LOOBACK_CHECK_DATA,
    SYSEX_LOOBACK_CHECK_DATA,
    SYSEX_LOOBACK_CHECK_DATA
  };
  sendSysEx(5, msgBogusLoopbackData);

  // Send the firmware version data
  static const byte msgRequestFirmwareVersion[] = {
    AXE_MANUFACTURER,
    1,
    SYSEX_AXEFX_FIRMWARE_VERSION,
    0,
    0
  };
  sendSysEx(7, (byte*)msgRequestFirmwareVersion);

}

/**
 * Tell the AxeFx to send the PresetName over Midi
 */
void AxeMidi_Class::requestPresetName() {
  static const byte msgRequestPresetName[] = {
    AXE_MANUFACTURER,
    m_iAxeModel,
    SYSEX_AXEFX_PRESET_NAME
  };
  sendSysEx(5, (byte*)msgRequestPresetName);
}

/**
 * Tell the AxeFx to send the PresetName over Midi
 * @TODO This is just a test, no clue if it actually works.
 * but we really want to know the current presetnumber on startup
 * so lets just try.
 */
void AxeMidi_Class::requestPresetNumber() {
  static const byte msgRequestPresetNumber[] = {
    AXE_MANUFACTURER,
    m_iAxeModel,
    SYSEX_AXEFX_PRESET_CHANGE
  };
  sendSysEx(5, (byte*)msgRequestPresetNumber);
}

/**
 * Tell the AxeFx to send the bypass states for the current preset's effects
 * http://forum.fractalaudio.com/other-midi-controllers/39161-using-sysex-recall-present-effect-bypass-status-info-available.html
 */
void AxeMidi_Class::requestBypassStates() {
  static const byte msgRequestBypassStates[] = {
    AXE_MANUFACTURER,
    m_iAxeModel,
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
int AxeMidi_Class::getModel() {
  return m_iAxeModel;
}

/**
 * Registering of the callback functions
 */
void AxeMidi_Class::registerAxeSysExReceiveCallback( void (*func)(byte*,int) ) {
  m_fpAxeFxSysExCallback = func;
}
void AxeMidi_Class::registerRawSysExReceiveCallback( void (*func)(byte*,int) ) {
  m_fpRawSysExCallback = func;
}