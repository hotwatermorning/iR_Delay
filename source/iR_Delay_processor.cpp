//------------------------------------------------------------------------
// Copyright(c) 2021 iNVOX Records.
//------------------------------------------------------------------------

#include "iR_Delay_processor.h"
#include "iR_Delay_cids.h"
#include "iR_Delay_paramids.h"
#include "iR_Utility.h"

#include "base/source/fstreamer.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "public.sdk/source/vst/vstaudioprocessoralgo.h"

#include <cmath>
#include <algorithm>

namespace INVOXRecords {
//------------------------------------------------------------------------
// iR_DelayProcessor
//------------------------------------------------------------------------
iR_DelayProcessor::iR_DelayProcessor()
{
  setControllerClass(kiR_DelayControllerUID);
}

//------------------------------------------------------------------------
iR_DelayProcessor::~iR_DelayProcessor()
{
  delete delay;
}

//------------------------------------------------------------------------
tresult PLUGIN_API iR_DelayProcessor::initialize(FUnknown* context)
{
  tresult result = AudioEffect::initialize(context);
  if (result != kResultOk) return result;

  addAudioInput (STR16 ("Stereo In"), Steinberg::Vst::SpeakerArr::kStereo);
	addAudioOutput (STR16 ("Stereo Out"), Steinberg::Vst::SpeakerArr::kStereo);

  processContextRequirements.needTempo();
  if (!delay) delay = new Delay(44100.0f, 4.0f);

  return kResultOk;
}

tresult PLUGIN_API iR_DelayProcessor::setBusArrangements(
  Vst::SpeakerArrangement* inputs, int32 numIns,
  Vst::SpeakerArrangement* outputs, int32 numOuts)
{
  if (numIns == 1 && numOuts == 1
   && inputs[0] == Vst::SpeakerArr::kStereo
   && outputs[0] == Vst::SpeakerArr::kStereo) {
    return AudioEffect::setBusArrangements(inputs, numIns, outputs, numOuts);
  } else if (numIns == 1 && numOuts == 1 
          && inputs[0] == Vst::SpeakerArr::kMono
          && outputs[0] == Vst::SpeakerArr::kMono) {
    return AudioEffect::setBusArrangements(inputs, numIns, outputs, numOuts);
  } else if (numIns == 1 && numOuts == 1
          && inputs[0] == Vst::SpeakerArr::kStereo
          && outputs[0] == Vst::SpeakerArr::kMono) {
    return AudioEffect::setBusArrangements(inputs, numIns, outputs, numOuts);
  } else if (numIns == 1 && numOuts == 1
          && inputs[0] == Vst::SpeakerArr::kMono
          && outputs[0] == Vst::SpeakerArr::kStereo) {
    return AudioEffect::setBusArrangements(inputs, numIns, outputs, numOuts);
  }

  return kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API iR_DelayProcessor::terminate()
{
  return AudioEffect::terminate();
}

//------------------------------------------------------------------------
tresult PLUGIN_API iR_DelayProcessor::setActive(TBool state)
{
  return AudioEffect::setActive(state);
}

//------------------------------------------------------------------------
tresult PLUGIN_API iR_DelayProcessor::process(Vst::ProcessData& data)
{
  if (data.inputParameterChanges) {
    int32 numParamsChanged = data.inputParameterChanges->getParameterCount();
    for (int32 index = 0; index < numParamsChanged; index++) {
      if (auto* paramQueue = data.inputParameterChanges->getParameterData(index)) {
        Vst::ParamValue value;
        int32 sampleOffset;
        int32 numPoints = paramQueue->getPointCount();
        switch (paramQueue->getParameterId())
        {
        case kBypassID:
          if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
            bBypass = value > 0.5f;
          }
          break;
        case kEnableBPMSyncID:
          if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
            delay->setEnableBPMSync(value > 0.5f);
          }
          break;
        case kBeatSelectLeftID:
          if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
            delay->setSyncBeat((DelaySyncBeat)(int)(std::fmin(20.0f, (float)value * 21)), DelayChannel::kLeftChannel);
          }
          break;
        case kBeatSelectRightID:
          if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
            delay->setSyncBeat((DelaySyncBeat)(int)(std::fmin(20.0f, (float)value * 21)), DelayChannel::kRightChannel);
          }
          break;
        case kDelayTimeLeftID:
          if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
            delay->setTime(value * 3.99f + 0.01f, DelayChannel::kLeftChannel);
          }
          break;
        case kDelayTimeRightID:
          if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
            delay->setTime(value * 3.99f + 0.01f, DelayChannel::kRightChannel);
          }
          break;
        case kEnableTimeLinkID:
          if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
            delay->setEnableTimeLink(value > 0.5f);
          }
          break;
        case kFeedbackLeftID:
          if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
            delay->setFeedback((value * 99.0f) / 100.0f, DelayChannel::kLeftChannel);
          }
          break;
        case kFeedbackRightID:
          if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
            delay->setFeedback((value * 99.0f) / 100.0f, DelayChannel::kRightChannel);
          }
          break;
        case kEnableFeedbackLinkID:
          if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
            delay->setEnableFeedbackLink(value > 0.5f);
          }
          break;
        case kDumpingID:
          if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
            delay->setDumping(value);
          }
          break;
        case kHighCutID:
          if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
            delay->setLPFFreq(value * 19000.0f + 1000.0f);
          }
          break;
        case kLowCutID:
          if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
            delay->setHPFFreq(value * 980.0f + 20.0f);
          }
          break;
        case kEnablePingPongID:
          if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
            if (value > 0.5f)
              delay->setMode(DelayMode::kPingpong);
            else
              delay->setMode(DelayMode::kNormal);
          }
          break;
        case kEnableSwapChannelID:
          if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
            delay->setEnableSwapChannel(value > 0.5f);
          }
          break;
        case kDuckingID:
          if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
            delay->setDucking(value);
          }
          break;
        case kMixID:
          if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
            delay->setMix(value);
          }
          break;
        case kStereoWidthID:
          if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
            delay->setStereoWidth(value * 2);
          }
          break;
        case kOutputGainID:
          if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
            gain = value * 12.0f - 6.0f;
          }
          break;
        }
      }
    }
  }

  //--- Here you have to implement your processing
  if (data.numInputs == 0 || data.numOutputs == 0) return kResultOk;

  int32 numChannels = data.inputs[0].numChannels;
  uint32 sampleFramesSize = getSampleFramesSizeInBytes(processSetup, data.numSamples);
  void** in = getChannelBuffersPointer(processSetup, data.inputs[0]);
  void** out = getChannelBuffersPointer(processSetup, data.outputs[0]);

  if (data.inputs[0].silenceFlags != 0) {
    data.outputs[0].silenceFlags = data.inputs[0].silenceFlags;

    for (int32 i = 0; i < numChannels; i++)
      if (in[i] != out[i]) 
        memset(out[i], 0, sampleFramesSize);
    return kResultOk;
  }

  data.outputs[0].silenceFlags = 0;

  if (bBypass) {
    for (int32 i = 0; i < numChannels; i++)
      if (in[i] != out[i]) 
        memcpy(out[i], in[i], sampleFramesSize);
  } else {
    int32 samples = data.numSamples;
    Vst::Sample32* ptrIn_L = (Vst::Sample32*)in[0];
    Vst::Sample32* ptrIn_R = (Vst::Sample32*)in[1];
    Vst::Sample32* ptrOut_L = (Vst::Sample32*)out[0];
    Vst::Sample32* ptrOut_R = (Vst::Sample32*)out[1];

    samplerate = processSetup.sampleRate;
    if (data.processContext && data.processContext->state & data.processContext->kTempoValid) {
      delay->setTempo(data.processContext->tempo);
    }
    delay->calculate();

    if (data.numInputs == 1 && data.numOutputs == 1) {
      while (--samples >= 0) {
        // in: stereo, out: stereo
        if (data.inputs[0].numChannels == 2 && data.outputs[0].numChannels == 2) {
          in_L = (*ptrIn_L++);
          in_R = (*ptrIn_R++);

          delay->applyDelay(&in_L, &in_R);

          setGain(gain, &in_L);
          setGain(gain, &in_R);

          (*ptrOut_L++) = in_L;
          (*ptrOut_R++) = in_R;
        // in: mono, out: mono
        } else if (data.inputs[0].numChannels == 1 && data.outputs[0].numChannels == 1) {
          in_L = (*ptrIn_L++);

          delay->applyDelay(&in_L);

          setGain(gain, &in_L);

          (*ptrOut_L++) = in_L;
        // in: stereo, out: mono
        } else if (data.inputs[0].numChannels == 2 && data.outputs[0].numChannels == 1) {
          in_L = ((*ptrIn_L++) + (*ptrIn_R++)) / 2.0f;

          delay->applyDelay(&in_L);

          setGain(gain, &in_L);

          (*ptrOut_L++) = in_L;
        // in: mono, out: stereo
        } else if (data.inputs[0].numChannels == 1 && data.outputs[0].numChannels == 2) {
          in_L = in_R = (*ptrIn_L++);

          delay->applyDelay(&in_L, &in_R);

          setGain(gain, &in_L);
          setGain(gain, &in_R);

          (*ptrOut_L++) = in_L;
          (*ptrOut_R++) = in_R;
        }
      }
    }
  }
  return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API iR_DelayProcessor::setupProcessing(Vst::ProcessSetup& newSetup)
{
  samplerate = this->processSetup.sampleRate;
  if (!delay) delay = new Delay(samplerate, 4.0f);

  return AudioEffect::setupProcessing(newSetup);
}

//------------------------------------------------------------------------
tresult PLUGIN_API iR_DelayProcessor::canProcessSampleSize(int32 symbolicSampleSize)
{
  if (symbolicSampleSize == Vst::kSample32) return kResultTrue;

  return kResultFalse;
}

// load value from preset
tresult PLUGIN_API iR_DelayProcessor::setState(IBStream* state)
{
  if (!state) return kResultFalse;
  /**/
  bool saved_bypass = false;
  float saved_output_gain = 0;
  
  bool saved_bpm_sync = false;
  float saved_beat_L = 0;
  float saved_beat_R = 0;
  float saved_time_L = 0;
  float saved_time_R = 0;
  bool saved_time_link = false;

  float saved_feedback_L = 0;
  float saved_feedback_R = 0;
  bool saved_feedback_link = false;

  float saved_dumping = 0;
  float saved_highcut = 0;
  float saved_lowcut = 0;

  bool saved_pingpong = false;
  bool saved_swap_channel = false;

  float saved_stereo_width = 0;
  float saved_ducking = 0;
  float saved_mix = 0;

  IBStreamer streamer(state, kLittleEndian);
  if (!streamer.readBool(saved_bypass)) return kResultFalse;
  if (!streamer.readFloat(saved_output_gain)) return kResultFalse;

  if (!streamer.readBool(saved_bpm_sync)) return kResultFalse;
  if (!streamer.readFloat(saved_beat_L)) return kResultFalse;
  if (!streamer.readFloat(saved_beat_R)) return kResultFalse;
  if (!streamer.readFloat(saved_time_L)) return kResultFalse;
  if (!streamer.readFloat(saved_time_R)) return kResultFalse;
  if (!streamer.readBool(saved_time_link)) return kResultFalse;

  if (!streamer.readFloat(saved_feedback_L)) return kResultFalse;
  if (!streamer.readFloat(saved_feedback_R)) return kResultFalse;
  if (!streamer.readBool(saved_feedback_link)) return kResultFalse;

  if (!streamer.readFloat(saved_dumping)) return kResultFalse;
  if (!streamer.readFloat(saved_highcut)) return kResultFalse;
  if (!streamer.readFloat(saved_lowcut)) return kResultFalse;

  if (!streamer.readBool(saved_pingpong)) return kResultFalse;
  if (!streamer.readBool(saved_swap_channel)) return kResultFalse;

  if (!streamer.readFloat(saved_stereo_width)) return kResultFalse;
  if (!streamer.readFloat(saved_ducking)) return kResultFalse;
  if (!streamer.readFloat(saved_mix)) return kResultFalse;

  bBypass = saved_bypass;
  gain = saved_output_gain;

  if (delay) {
    delay->setEnableBPMSync(saved_bpm_sync);
    delay->setSyncBeat((DelaySyncBeat)saved_beat_L, DelayChannel::kLeftChannel);
    delay->setSyncBeat((DelaySyncBeat)saved_beat_R, DelayChannel::kRightChannel);
    delay->setTime(saved_time_L, DelayChannel::kLeftChannel);
    delay->setTime(saved_time_R, DelayChannel::kRightChannel);
    delay->setEnableTimeLink(saved_time_link);

    delay->setFeedback(saved_feedback_L, DelayChannel::kLeftChannel);
    delay->setFeedback(saved_feedback_R, DelayChannel::kRightChannel);
    delay->setEnableFeedbackLink(saved_feedback_link);

    delay->setDumping(saved_dumping);
    delay->setLPFFreq(saved_highcut);
    delay->setHPFFreq(saved_lowcut);

    if (saved_pingpong) delay->setMode(DelayMode::kPingpong);
    else delay->setMode(DelayMode::kNormal);
    delay->setEnableSwapChannel(saved_swap_channel);

    delay->setStereoWidth(saved_stereo_width);
    delay->setDucking(saved_ducking);
    delay->setMix(saved_mix);
  }
  /**/
  return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API iR_DelayProcessor::getState(IBStream* state)
{
  if (!state) return kResultFalse;

  bool save_bypass = bBypass;
  float save_output_gain = gain;

  bool save_bpm_sync = delay->getEnableBPMSync();
  float save_beat_L = (float)delay->getSyncBeat(DelayChannel::kLeftChannel);
  float save_beat_R = (float)delay->getSyncBeat(DelayChannel::kRightChannel);
  float save_time_L = delay->getTime(DelayChannel::kLeftChannel);
  float save_time_R = delay->getTime(DelayChannel::kRightChannel);
  bool save_time_link = delay->getEnableTimeLink();

  float save_feedback_L = delay->getFeedback(DelayChannel::kLeftChannel);
  float save_feedback_R = delay->getFeedback(DelayChannel::kRightChannel);
  bool save_feedback_link = delay->getEnableFeedbackLink();

  float save_dumping = delay->getDumping();
  float save_highcut = delay->getLPFFreq();
  float save_lowcut = delay->getHPFFreq();

  bool save_pingpong = (int)delay->getMode() == (int)DelayMode::kPingpong;
  bool save_swap_channel = delay->getEnableSwapChannel();

  float save_stereo_width = delay->getStereoWidth();
  float save_ducking = delay->getDucking();
  float save_mix = delay->getMix();

  IBStreamer streamer(state, kLittleEndian);
  streamer.writeBool(save_bypass);
  streamer.writeFloat(save_output_gain);

  streamer.writeBool(save_bpm_sync);
  streamer.writeFloat(save_beat_L);
  streamer.writeFloat(save_beat_R);
  streamer.writeFloat(save_time_L);
  streamer.writeFloat(save_time_R);
  streamer.writeBool(save_time_link);

  streamer.writeFloat(save_feedback_L);
  streamer.writeFloat(save_feedback_R);
  streamer.writeBool(save_feedback_link);

  streamer.writeFloat(save_dumping);
  streamer.writeFloat(save_highcut);
  streamer.writeFloat(save_lowcut);

  streamer.writeBool(save_swap_channel);
  streamer.writeBool(save_pingpong);

  streamer.writeFloat(save_stereo_width);
  streamer.writeFloat(save_ducking);
  streamer.writeFloat(save_mix);
  
  return kResultOk;
}

//------------------------------------------------------------------------
} // namespace INVOXRecords
