/**
 * The EffectManager provides an interface to control the effects of the AxeFx
 * In the AxeFx-II there are over 70 effect blocks that can have various different parameters
 * This first version only controls the parameters all the different effects share, such as
 * bypass, XY toggle, etc. A newer version will be able to see the effect type and would allow
 * one to control the various parameters per effect block.
 * version 1.0 by mackatack@gmail.com
 * Released into the public domain.
 *
 * Use at own risk.
 */

 // @TODO: Add documentation and move functions to separate file

#include <WProgram.h>
#include "io_AxeMidi.h"

#ifndef FCBInfinity_EFFECTS_H
#define FCBInfinity_EFFECTS_H


// Here's a list of all the EffectID's that one can use
#define AXEFX_EFFECTID_Dummy 0
#define AXEFX_EFFECTID_Compressor1 100
#define AXEFX_EFFECTID_Compressor2 101
#define AXEFX_EFFECTID_GraphicEQ1 102
#define AXEFX_EFFECTID_GraphicEQ2 103
#define AXEFX_EFFECTID_ParametricEQ1 104
#define AXEFX_EFFECTID_ParametricEQ2 105
#define AXEFX_EFFECTID_Amp1 106
#define AXEFX_EFFECTID_Amp2 107
#define AXEFX_EFFECTID_Cabinet1 108
#define AXEFX_EFFECTID_Cabinet2 109
#define AXEFX_EFFECTID_Reverb1 110
#define AXEFX_EFFECTID_Reverb2 111
#define AXEFX_EFFECTID_Delay1 112
#define AXEFX_EFFECTID_Delay2 113
#define AXEFX_EFFECTID_Multidelay1 114
#define AXEFX_EFFECTID_Multidelay2 115
#define AXEFX_EFFECTID_Chorus1 116
#define AXEFX_EFFECTID_Chorus2 117
#define AXEFX_EFFECTID_Flanger1 118
#define AXEFX_EFFECTID_Flanger2 119
#define AXEFX_EFFECTID_Rotary1 120
#define AXEFX_EFFECTID_Rotary2 121
#define AXEFX_EFFECTID_Phaser1 122
#define AXEFX_EFFECTID_Phaser2 123
#define AXEFX_EFFECTID_Wahwah1 124
#define AXEFX_EFFECTID_Wahwah2 125
#define AXEFX_EFFECTID_Formant 126
#define AXEFX_EFFECTID_VolPan1 127
#define AXEFX_EFFECTID_PannerTremolo1 128
#define AXEFX_EFFECTID_PannerTremolo2 129
#define AXEFX_EFFECTID_Pitch1 130
#define AXEFX_EFFECTID_Filter1 131
#define AXEFX_EFFECTID_Filter2 132
#define AXEFX_EFFECTID_Drive1 133
#define AXEFX_EFFECTID_Drive2 134
#define AXEFX_EFFECTID_Enhancer 135
#define AXEFX_EFFECTID_EffectsLoop 136
#define AXEFX_EFFECTID_Mixer1 137
#define AXEFX_EFFECTID_Mixer2 138
#define AXEFX_EFFECTID_NoiseGate 139
#define AXEFX_EFFECTID_Output 140
#define AXEFX_EFFECTID_Controllers 141
#define AXEFX_EFFECTID_FeedbackSend 142
#define AXEFX_EFFECTID_FeedbackReturn 143
#define AXEFX_EFFECTID_Synth1 144
#define AXEFX_EFFECTID_Synth2 145
#define AXEFX_EFFECTID_Vocoder 146
#define AXEFX_EFFECTID_MegatapDelay 147
#define AXEFX_EFFECTID_Crossover1 148
#define AXEFX_EFFECTID_Crossover2 149
#define AXEFX_EFFECTID_GateExpander1 150
#define AXEFX_EFFECTID_GateExpander2 151
#define AXEFX_EFFECTID_RingMod 152
#define AXEFX_EFFECTID_Pitch2 153
#define AXEFX_EFFECTID_MultibandComp1 154
#define AXEFX_EFFECTID_MultibandComp2 155
#define AXEFX_EFFECTID_QuadChorus1 156
#define AXEFX_EFFECTID_QuadChorus2 157
#define AXEFX_EFFECTID_Resonator1 158
#define AXEFX_EFFECTID_Resonator2 159
#define AXEFX_EFFECTID_GraphicEQ3 160
#define AXEFX_EFFECTID_GraphicEQ4 161
#define AXEFX_EFFECTID_ParametricEQ3 162
#define AXEFX_EFFECTID_ParametricEQ4 163
#define AXEFX_EFFECTID_Filter3 164
#define AXEFX_EFFECTID_Filter4 165
#define AXEFX_EFFECTID_VolPan2 166
#define AXEFX_EFFECTID_VolPan3 167
#define AXEFX_EFFECTID_VolPan4 168
#define AXEFX_EFFECTID_Looper 169
#define AXEFX_EFFECTID_ToneMatch 170


// Some default CC settings for the AxeFx
// see http://wiki.fractalaudio.com/axefx2/index.php?title=MIDI_CCs_list
#define AXEFX_DEFAULTCC_Input_Volume 10
#define AXEFX_DEFAULTCC_Out_1_Volume 11
#define AXEFX_DEFAULTCC_Out_2_Volume 12
#define AXEFX_DEFAULTCC_Bypass 13
#define AXEFX_DEFAULTCC_Tempo 14
#define AXEFX_DEFAULTCC_Tuner 15
#define AXEFX_DEFAULTCC_External_Control_1 16
#define AXEFX_DEFAULTCC_External_Control_2 17
#define AXEFX_DEFAULTCC_External_Control_3 18
#define AXEFX_DEFAULTCC_External_Control_4 19
#define AXEFX_DEFAULTCC_External_Control_5 20
#define AXEFX_DEFAULTCC_External_Control_6 21
#define AXEFX_DEFAULTCC_External_Control_7 22
#define AXEFX_DEFAULTCC_External_Control_8 23
#define AXEFX_DEFAULTCC_External_Control_9 24
#define AXEFX_DEFAULTCC_External_Control_10 25
#define AXEFX_DEFAULTCC_External_Control_11 26
#define AXEFX_DEFAULTCC_External_Control_12 27

#define AXEFX_DEFAULTCC_Looper_Record 28
#define AXEFX_DEFAULTCC_Looper_Play 29
#define AXEFX_DEFAULTCC_Looper_Once 30
#define AXEFX_DEFAULTCC_Looper_Dub 31
#define AXEFX_DEFAULTCC_Looper_Rev 32
#define AXEFX_DEFAULTCC_Looper_Bypass 33
#define AXEFX_DEFAULTCC_Looper_HalfSpeed 120
#define AXEFX_DEFAULTCC_Looper_Undo 121
#define AXEFX_DEFAULTCC_Looper_Metronome 122

#define AXEFX_DEFAULTCC_Global_Bypass 34
#define AXEFX_DEFAULTCC_Volume_Increment 35
#define AXEFX_DEFAULTCC_Volume_Decrement 36

// No need to define the bypass CCs since the FCBEffect objects will contain the
// actual values sent from the AxeFx.

#define AXEFX_DEFAULTCC_Amp_1_XY 100
#define AXEFX_DEFAULTCC_Amp_2_XY 101
#define AXEFX_DEFAULTCC_Cab_1_XY 102
#define AXEFX_DEFAULTCC_Cab_2_XY 103
#define AXEFX_DEFAULTCC_Chorus_1_XY 104
#define AXEFX_DEFAULTCC_Chorus_2_XY 105
#define AXEFX_DEFAULTCC_Delay_1_XY 106
#define AXEFX_DEFAULTCC_Delay_2_XY 107
#define AXEFX_DEFAULTCC_Drive_1_XY 108
#define AXEFX_DEFAULTCC_Drive_2_XY 109
#define AXEFX_DEFAULTCC_Flanger_1_XY 110
#define AXEFX_DEFAULTCC_Flanger_2_XY 111
#define AXEFX_DEFAULTCC_Phaser_1_XY 112
#define AXEFX_DEFAULTCC_Phaser_2_XY 113
#define AXEFX_DEFAULTCC_Pitch_1_XY 114
#define AXEFX_DEFAULTCC_Pitch_2_XY 115
#define AXEFX_DEFAULTCC_Reverb_1_XY 116
#define AXEFX_DEFAULTCC_Reverb_2_XY 117
#define AXEFX_DEFAULTCC_Wahwah_1_XY 118
#define AXEFX_DEFAULTCC_Wahwah_2_XY 119

// How many effects do we have? Used for initializing the array later on
#define AXEFX_EFFECT_FIRST    AXEFX_EFFECTID_Compressor1
#define AXEFX_EFFECT_LAST     AXEFX_EFFECTID_ToneMatch
#define AXEFX_EFFECT_COUNT    AXEFX_EFFECT_LAST - AXEFX_EFFECT_FIRST + 1

// Defines the various states an effectblock might have
#define AXEFX_EFFECT_STATE_ACTIVE 1
#define AXEFX_EFFECT_STATE_X 2

// Taken from the ../documentation/AxeFX Effect ID List.csv file
const int effectTypes[] = {
  2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13,14,
  14,15,34,16,16,17,18,18,19,19,20,21,22,22,35,36,37,23,24,25,
  25,26,27,28,28,29,29,30,17,31,31,32,32,33,33,3,3,4,4,18,18,34,
  34,34,38,39
};

/**
 * This class stores information about effects, such as their
 * state, cc#, effectID and has functions implemented that know
 * what CC, PC or SysEx midi message to send to get or set specific
 * parameters.
 */
class FCBEffect {
public:
  FCBEffect(int effectID, int typeID) {
    this->effectID = effectID;
    this->typeID = typeID;
    state = -1;
    cc = 0;
  }
  int getEffectID() { return effectID; }
  int getType() { return typeID; }
  int getCC() { return cc; }
  void setCC(int cc) { this->cc = cc; }
  int getState() { return state; }
  void setState(int state) { this->state = state; }
  void setStateAndCC(int state, int cc) {
    this->state = state;
    this->cc = cc;
    /*Serial.print("EffectID ");
    Serial.print(effectID);
    Serial.print(" state: ");
    Serial.print(state);
    Serial.print(" X: ");
    Serial.print(isXMode());
    Serial.print(" Active: ");
    Serial.print(isActive());
    Serial.print(" cc: ");
    Serial.println(cc);*/
  }
  bool isPlaced() { return state != -1; }
  bool isActive() { return isPlaced() && (state & AXEFX_EFFECT_STATE_ACTIVE) == AXEFX_EFFECT_STATE_ACTIVE; }
  bool isXMode() { return isPlaced() && (state & AXEFX_EFFECT_STATE_X) == AXEFX_EFFECT_STATE_X; }
  bool toggleActive() { setActive(!isActive()); }
  bool activate() { setActive(true); }
  bool deactivate() { setActive(false); }
  void updateParams() {
    Serial.println("SUPERCLASS Updating looper params");
  };
  void handleParamUpdate(int paramID, int value) {};
  void setActive(bool active);
  void setY(bool active, int bypassCC);
protected:
  int effectID;
  int typeID;
  int cc;
  int state;
};

/**
 * Custom class for handling the Looper Block's parameters
 */
class FCBLooperEffect_Class: public FCBEffect {
  protected:
    bool bPlay, bRecord, bOnce, bDub, bRev, bBypass, bHalfSpeed, bUndo, bMetronome;
  public:
  FCBLooperEffect_Class(int effectID, int typeID):
    FCBEffect::FCBEffect(effectID, typeID) {
    bPlay, bRecord, bOnce, bDub, bRev, bBypass, bHalfSpeed, bUndo, bMetronome = false;
  }
  void updateStatus(int status) {
    bRecord = (status & B00000001) == B00000001;
    bPlay = (status & B00000010) == B00000010;
    bOnce = (status & B00000100) == B00000100;
    bDub = (status & B00001000) == B00001000;
    bRev = (status & B00010000) == B00010000;
    bHalfSpeed = (status & B00100000) == B00100000;
    bUndo = (status & B01000000) == B01000000;
  }
  void setPlay(bool state) {
    bPlay = state;
    AxeMidi.sendControlChange(AXEFX_DEFAULTCC_Looper_Play, state?127:0);
  }
  void setRecord(bool state) {
    bRecord = state;
    AxeMidi.sendControlChange(AXEFX_DEFAULTCC_Looper_Record, state?127:0);
  }
  void setOnce(bool state) {
    bOnce = state;
    AxeMidi.sendControlChange(AXEFX_DEFAULTCC_Looper_Once, state?127:0);
  }
  void setDub(bool state) {
    bDub = state;
    AxeMidi.sendControlChange(AXEFX_DEFAULTCC_Looper_Dub, state?127:0);
  }
  void setReverse(bool state) {
    bRev = state;
    AxeMidi.sendControlChange(AXEFX_DEFAULTCC_Looper_Rev, state?127:0);
  }
  void setBypass(bool state) {
    bBypass = state;
    AxeMidi.sendControlChange(AXEFX_DEFAULTCC_Looper_Bypass, state?127:0);
  }
  void setHalfSpeed(bool state) {
    bHalfSpeed = state;
    AxeMidi.sendControlChange(AXEFX_DEFAULTCC_Looper_HalfSpeed, state?127:0);
  }
  void setUndo(bool state) {
    bUndo = state;
    AxeMidi.sendControlChange(AXEFX_DEFAULTCC_Looper_Undo, state?127:0);
  }
  void setMetronome(bool state) {
    bMetronome = state;
    AxeMidi.sendControlChange(AXEFX_DEFAULTCC_Looper_Metronome, state?127:0);
  }

  bool getPlay() {
    return bPlay;
  }
  bool getRecord() {
    return bRecord;
  }
  bool getOnce() {
    return bOnce;
  }
  bool getDub() {
    return bDub;
  }
  bool getReverse() {
    return bRev;
  }
  bool getBypass() {
    return bBypass;
  }
  bool getHalfSpeed() {
    return bHalfSpeed;
  }
  bool getUndo() {
    return bUndo;
  }
  bool getMetronome() {
    return bMetronome;
  }
};
extern FCBLooperEffect_Class FCBLooperEffect;

/**
 * This static class stores all the FCBEffect objects that control the various
 * different effect block on the AxeFx. If we check ../documentation/AxeFX Effect ID List.csv
 * we can see that currently the effectblocks range from effectID 100 to 170 (excluding the dummy)
 * This object just keeps an array of FCBEffects for easy access
 */
class FCBEffectManager_Class {
public:
  void setStatesStale() {
    m_bEffectManagerStatesStale = true;
  }

  void updateIfStale() {
    if (!m_bEffectManagerStatesStale) return;
    m_bEffectManagerStatesStale = false;
    AxeMidi.requestBypassStates();
  }

  FCBEffectManager_Class() {
    m_bEffectManagerStatesStale = true;

    for (int i=0; i<=AXEFX_EFFECT_COUNT; ++i) {
      //if (i == AXEFX_EFFECTID_Looper) {
      //  effects[i] = (FCBEffect*)&FCBLooperEffect;
      //}
      //else {
        effects[i] = new FCBEffect(i+AXEFX_EFFECT_FIRST, effectTypes[i]);
      //}
    }
    dummy = new FCBEffect(0, 0);;
  }

  // This [] operator returns the corresponding effect and makes sure
  // it always returns something, even if the effect doesnt exist. In that
  // case it just returns the dummy
  // Can be used as follows:
  // FCBEffectManager[effectID]->state
  // or
  // FCBEffectManager[AXEFX_EFFECTID_Drive1]->setBypassed(false)
  FCBEffect* operator[](int effectID) {
    if (effectID<AXEFX_EFFECT_FIRST)
      return dummy;
    if (effectID>AXEFX_EFFECT_LAST)
      return dummy;
    return effects[effectID - AXEFX_EFFECT_FIRST];
  }

  /**
   * Sets the states of all the effectblocks to -1 (not placed)
   */
  void resetStates() {
    for (int i=0; i<AXEFX_EFFECT_COUNT; i++) {
      effects[i]->setState(-1);
    }
  }
private:
  FCBEffect * effects[AXEFX_EFFECT_COUNT];
  FCBEffect * dummy;
  bool m_bEffectManagerStatesStale;
};
extern FCBEffectManager_Class FCBEffectManager;

#endif