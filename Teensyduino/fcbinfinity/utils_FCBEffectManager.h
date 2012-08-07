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

#define AXEFX_EFFECT_FIRST    AXEFX_EFFECTID_Compressor1
#define AXEFX_EFFECT_LAST     AXEFX_EFFECTID_ToneMatch
#define AXEFX_EFFECT_COUNT    AXEFX_EFFECT_LAST - AXEFX_EFFECT_FIRST + 1

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
    Serial.print("EffectID ");
    Serial.print(effectID);
    Serial.print(" state: ");
    Serial.print(state);
    Serial.print(" cc: ");
    Serial.println(cc);
  }
  void setBypassed(bool newBypassState) {
    AxeMidi.sendControlChange(cc, newBypassState?127:0);
  }
protected:
  int effectID;
  int typeID;
  int cc;
  int state;
};

/**
 * This static class stores all the FCBEffect objects that control the various
 * different effect block on the AxeFx. If we check ../documentation/AxeFX Effect ID List.csv
 * we can see that currently the effectblocks range from effectID 100 to 170 (excluding the dummy)
 * This object just keeps an array of FCBEffects for easy access
 */
class FCBEffectManager_Class {
public:
  FCBEffectManager_Class() {
    for (int i=0; i<=AXEFX_EFFECT_COUNT; ++i) {
      effects[i] = new FCBEffect(i+AXEFX_EFFECT_FIRST, effectTypes[i]);
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
};

extern FCBEffectManager_Class FCBEffectManager;

#endif