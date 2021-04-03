//------------------------------------------------------------------------
// Copyright(c) 2021 iNVOX Records.
//------------------------------------------------------------------------

#pragma once

#include "RingBuffer.h"
#include "iR_Filters.h"
#include "iR_Delay.h"
#include "iR_RMSDetector.h"

#include "base/source/timer.h"
#include "public.sdk/source/vst/vstaudioeffect.h"

using namespace Steinberg;

namespace INVOXRecords {
//------------------------------------------------------------------------
//  iR_DelayProcessor
//------------------------------------------------------------------------
class iR_DelayProcessor : public Vst::AudioEffect
{
 public:
  iR_DelayProcessor();
  ~iR_DelayProcessor() SMTG_OVERRIDE;

  static FUnknown* createInstance(void* /*context*/)
  {
    return (Vst::IAudioProcessor*)new iR_DelayProcessor;
  }

  tresult PLUGIN_API initialize(FUnknown* context) SMTG_OVERRIDE;
  tresult PLUGIN_API setBusArrangements(Vst::SpeakerArrangement* inputs, int32 numIns,
                                        Vst::SpeakerArrangement* outputs, int32 numOuts) SMTG_OVERRIDE;
  tresult PLUGIN_API terminate() SMTG_OVERRIDE;
  tresult PLUGIN_API setActive(TBool state) SMTG_OVERRIDE;
  tresult PLUGIN_API setupProcessing(Vst::ProcessSetup& newSetup) SMTG_OVERRIDE;
  tresult PLUGIN_API canProcessSampleSize(int32 symbolicSampleSize) SMTG_OVERRIDE;
  tresult PLUGIN_API process(Vst::ProcessData& data) SMTG_OVERRIDE;
  tresult PLUGIN_API setState(IBStream* state) SMTG_OVERRIDE;
  tresult PLUGIN_API getState(IBStream* state) SMTG_OVERRIDE;

//------------------------------------------------------------------------
 protected:
  float samplerate = 44100.0f;
  bool bBypass = false;
  float in_L = 0.0f, in_R = 0.0f;
  float gain = 0.0f;

  Delay* delay = nullptr;
};
//------------------------------------------------------------------------
} // namespace INVOXRecords
