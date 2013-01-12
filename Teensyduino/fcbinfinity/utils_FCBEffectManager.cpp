#include "utils_FCBEffectManager.h"

void FCBEffect::setY(bool active, int bypassCC) {
  //FCBEffectManager.setStatesStale();
  if (state == -1) return;
  if (active)
    state |= AXEFX_EFFECT_STATE_X;
  else
    state ^= AXEFX_EFFECT_STATE_X;
  AxeMidi.sendControlChange(bypassCC, (active)?127:0);
}

void FCBEffect::setActive(bool active) {
  //FCBEffectManager.setStatesStale();
  if (state == -1) return;
  if (active)
    state |= AXEFX_EFFECT_STATE_ACTIVE;
  else
    state ^= AXEFX_EFFECT_STATE_ACTIVE;
  AxeMidi.sendControlChange(cc, (active)?127:0);
}

FCBLooperEffect_Class FCBLooperEffect = FCBLooperEffect_Class(AXEFX_EFFECTID_Looper, effectTypes[AXEFX_EFFECTID_Looper-AXEFX_EFFECT_FIRST]);
FCBEffectManager_Class FCBEffectManager = FCBEffectManager_Class();