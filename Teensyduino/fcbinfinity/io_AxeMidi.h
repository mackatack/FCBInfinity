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

// Effect id's
// http://wiki.fractalaudio.com/index.php?title=Axe-Fx_SysEx_Documentation#Effect_Block_IDs
// Better yet, see the default.axeml file supplied with the AxeEdit program for all the correct ids
// Only added the common effect id's
#define SYSEX_AXEFX_EFFECTID_Comp1            0x0406
#define SYSEX_AXEFX_EFFECTID_Comp2            0x0506
#define SYSEX_AXEFX_EFFECTID_GEQ1	            0x0606
#define SYSEX_AXEFX_EFFECTID_GEQ2	            0x0706
#define SYSEX_AXEFX_EFFECTID_PEQ1	            0x0806
#define SYSEX_AXEFX_EFFECTID_PEQ2	            0x0906
#define SYSEX_AXEFX_EFFECTID_Amp1	            0x0A06
#define SYSEX_AXEFX_EFFECTID_Amp2	            0x0B06
#define SYSEX_AXEFX_EFFECTID_Cab1	            0x0C06
#define SYSEX_AXEFX_EFFECTID_Cab2	            0x0D06
#define SYSEX_AXEFX_EFFECTID_Reverb1	        0x0E06
#define SYSEX_AXEFX_EFFECTID_Reverb2	        0x0F06
#define SYSEX_AXEFX_EFFECTID_Delay1	          0x0007
#define SYSEX_AXEFX_EFFECTID_Delay2	          0x0107
#define SYSEX_AXEFX_EFFECTID_MultiDelay1	    0x0207
#define SYSEX_AXEFX_EFFECTID_MultiDelay2	    0x0307
#define SYSEX_AXEFX_EFFECTID_Chorus1	        0x0407
#define SYSEX_AXEFX_EFFECTID_Chorus2	        0x0507
#define SYSEX_AXEFX_EFFECTID_Flanger1	        0x0607
#define SYSEX_AXEFX_EFFECTID_Flanger2	        0x0707
#define SYSEX_AXEFX_EFFECTID_Rotary1	        0x0807
#define SYSEX_AXEFX_EFFECTID_Rotary2	        0x0907
#define SYSEX_AXEFX_EFFECTID_Phaser1	        0x0A07
#define SYSEX_AXEFX_EFFECTID_Phaser2	        0x0B07
#define SYSEX_AXEFX_EFFECTID_Wah1	            0x0C07
#define SYSEX_AXEFX_EFFECTID_Wah2	            0x0D07
#define SYSEX_AXEFX_EFFECTID_Formant	        0x0E07
#define SYSEX_AXEFX_EFFECTID_VolumePan	      0x0F07
#define SYSEX_AXEFX_EFFECTID_TremoloPan1	    0x0008
#define SYSEX_AXEFX_EFFECTID_TremoloPan2	    0x0108
#define SYSEX_AXEFX_EFFECTID_Pitch	          0x0208
#define SYSEX_AXEFX_EFFECTID_Filter1	        0x0308
#define SYSEX_AXEFX_EFFECTID_Filter2	        0x0408
#define SYSEX_AXEFX_EFFECTID_Drive1	          0x0508
#define SYSEX_AXEFX_EFFECTID_Drive2	          0x0608
#define SYSEX_AXEFX_EFFECTID_Enhancer	        0x0708
#define SYSEX_AXEFX_EFFECTID_FX_Loop	        0x0808
#define SYSEX_AXEFX_EFFECTID_Mixer1	          0x0908
#define SYSEX_AXEFX_EFFECTID_Mixer2	          0x0A08
#define SYSEX_AXEFX_EFFECTID_Feedback_Send	  0x0E08
#define SYSEX_AXEFX_EFFECTID_Feedback_Return  0x0F08

// These defines are used to extract the low and high byte
// from the effect id's above
#define EFFECTID_HIGH_BYTE(b) (b >> 8 & 0xFF)
#define EFFECTID_LOW_BYTE(b) (b & 0xFF)

// Some default CC settings for the AxeFx
// see http://wiki.fractalaudio.com/axefx2/index.php?title=MIDI_CCs_list
#define CC_AXEFX_Input_Volume 10
#define CC_AXEFX_Out_1_Volume 11
#define CC_AXEFX_Out_2_Volume 12
#define CC_AXEFX_Bypass 13
#define CC_AXEFX_Tempo 14
#define CC_AXEFX_Tuner 15
#define CC_AXEFX_External_Control_1 16
#define CC_AXEFX_External_Control_2 17
#define CC_AXEFX_External_Control_3 18
#define CC_AXEFX_External_Control_4 19
#define CC_AXEFX_External_Control_5 20
#define CC_AXEFX_External_Control_6 21
#define CC_AXEFX_External_Control_7 22
#define CC_AXEFX_External_Control_8 23
#define CC_AXEFX_External_Control_9 24
#define CC_AXEFX_External_Control_10 25
#define CC_AXEFX_External_Control_11 26
#define CC_AXEFX_External_Control_12 27

#define CC_AXEFX_Looper_Record 28
#define CC_AXEFX_Looper_Play 29
#define CC_AXEFX_Looper_Once 30
#define CC_AXEFX_Looper_Dub 31
#define CC_AXEFX_Looper_Rev 32
#define CC_AXEFX_Looper_Bypass 33
#define CC_AXEFX_Looper_Half-speed 120
#define CC_AXEFX_Looper_Undo 121
#define CC_AXEFX_Looper_Metronome 122

#define CC_AXEFX_Global_Bypass 34
#define CC_AXEFX_Volume_Increment 35
#define CC_AXEFX_Volume_Decrement 36

#define CC_AXEFX_Amp_1_Bypass 37
#define CC_AXEFX_Amp_2_Bypass 38
#define CC_AXEFX_Cab_1_Bypass 39
#define CC_AXEFX_Cab_2_Bypass 40
#define CC_AXEFX_Chorus_1_Bypass 41
#define CC_AXEFX_Chorus_2_Bypass 42
#define CC_AXEFX_Compressor_1_Bypass 43
#define CC_AXEFX_Compressor_2_Bypass 44
#define CC_AXEFX_Crossover_1_Bypass 45
#define CC_AXEFX_Crossover_2_Bypass 46
#define CC_AXEFX_Delay_1_Bypass 47
#define CC_AXEFX_Delay_2_Bypass 48
#define CC_AXEFX_Drive_1_Bypass 49
#define CC_AXEFX_Drive_2_Bypass 50
#define CC_AXEFX_Enhancer_Bypass 51
#define CC_AXEFX_Filter_1_Bypass 52
#define CC_AXEFX_Filter_2_Bypass 53
#define CC_AXEFX_Filter_3_Bypass 54
#define CC_AXEFX_Filter_4_Bypass 55
#define CC_AXEFX_Flanger_1_Bypass 56
#define CC_AXEFX_Flanger_2_Bypass 57
#define CC_AXEFX_Formant_1_Bypass 58
#define CC_AXEFX_FX_Loop_Bypass 59
#define CC_AXEFX_GateExpander_1_Bypass 60
#define CC_AXEFX_GateExpander_2_Bypass 61
#define CC_AXEFX_Graphic_EQ_1_Bypass 62
#define CC_AXEFX_Graphic_EQ_2_Bypass 63
#define CC_AXEFX_Graphic_EQ_3_Bypass 64
#define CC_AXEFX_Graphic_EQ_4_Bypass 65
#define CC_AXEFX_Megatap_Delay_Bypass 66

// Presumably these four were switched in the manual
#define CC_AXEFX_Multiband_Comp_1_Bypass 67 // manual: 69
#define CC_AXEFX_Multiband_Comp_2_Bypass 68 // manual: 70
#define CC_AXEFX_Multi_Delay_1_Bypass 69    // manual: 67
#define CC_AXEFX_Multi_Delay_2_Bypass 70    // manual: 68

#define CC_AXEFX_Parametric_EQ_1_Bypass 71
#define CC_AXEFX_Parametric_EQ_2_Bypass 72
#define CC_AXEFX_Parametric_EQ_3_Bypass 73
#define CC_AXEFX_Parametric_EQ_4_Bypass 74
#define CC_AXEFX_Phaser_1_Bypass 75
#define CC_AXEFX_Phaser_2_Bypass 76
#define CC_AXEFX_Pitch_Shifter_1_Bypass 77
#define CC_AXEFX_Pitch_Shifter_2_Bypass 78
#define CC_AXEFX_Quad_Chorus_1_Bypass 79
#define CC_AXEFX_Quad_Chorus_2_Bypass 80
#define CC_AXEFX_Resonator_1_Bypass 81
#define CC_AXEFX_Resonator_2_Bypass 82
#define CC_AXEFX_Reverb_1_Bypass 83
#define CC_AXEFX_Reverb_2_Bypass 84
#define CC_AXEFX_Ring_Modulator_Bypass 85
#define CC_AXEFX_Rotary_1_Bypass 86
#define CC_AXEFX_Rotary_2_Bypass 87
#define CC_AXEFX_Synth_1_Bypass 88
#define CC_AXEFX_Synth_2_Bypass 89
#define CC_AXEFX_Tremolo_1_Bypass 90
#define CC_AXEFX_Tremolo_2_Bypass 91
#define CC_AXEFX_Vocoder_Bypass 92
#define CC_AXEFX_VolumePan_1_Bypass 93
#define CC_AXEFX_VolumePan_2_Bypass 94
#define CC_AXEFX_VolumePan_3_Bypass 95
#define CC_AXEFX_VolumePan_4_Bypass 96
#define CC_AXEFX_Wahwah_1_Bypass 97
#define CC_AXEFX_Wahwah_2_Bypass 98
#define CC_AXEFX_Tone_Match_Bypass 99

#define CC_AXEFX_Amp_1_XY 100
#define CC_AXEFX_Amp_2_XY 101
#define CC_AXEFX_Cab_1_XY 102
#define CC_AXEFX_Cab_2_XY 103
#define CC_AXEFX_Chorus_1_XY 104
#define CC_AXEFX_Chorus_2_XY 105
#define CC_AXEFX_Delay_1_XY 106
#define CC_AXEFX_Delay_2_XY 107
#define CC_AXEFX_Drive_1_XY 108
#define CC_AXEFX_Drive_2_XY 109
#define CC_AXEFX_Flanger_1_XY 110
#define CC_AXEFX_Flanger_2_XY 111
#define CC_AXEFX_Phaser_1_XY 112
#define CC_AXEFX_Phaser_2_XY 113
#define CC_AXEFX_Pitch_1_XY 114
#define CC_AXEFX_Pitch_2_XY 115
#define CC_AXEFX_Reverb_1_XY 116
#define CC_AXEFX_Reverb_2_XY 117
#define CC_AXEFX_Wahwah_1_XY 118
#define CC_AXEFX_Wahwah_2_XY 119


//

class AxeMidi_Class: public MIDI_Class {
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

    /* gets and sets the midi channel this library will send messages on */
    void setMidiSendChannel(int channel) {m_iAxeChannel=channel;}
    int setMidiSendChannel() {return m_iAxeChannel;}

  protected:
    boolean m_bHasMessage;
    int m_iAxeModel;
    int m_iAxeChannel;
    boolean m_bFirmwareVersionReceived;

    /* Function pointer to a user defined function that handles raw sysex messages */
    void (*m_fpRawSysExCallback)(byte * sysex, int length);
    /* Function pointer to a user defined function that handles axefx sysex messages */
    void (*m_fpAxeFxSysExCallback)(byte * sysex, int length);
};

extern AxeMidi_Class AxeMidi;

#endif
