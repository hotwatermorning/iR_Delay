//------------------------------------------------------------------------
// Copyright(c) 2021 iNVOX Records.
//------------------------------------------------------------------------

#include "iR_Delay_controller.h"
#include "iR_Delay_cids.h"
#include "iR_Delay_paramids.h"
#include "base/source/fstreamer.h"
#include "vstgui/plugin-bindings/vst3editor.h"

namespace INVOXRecords {

//------------------------------------------------------------------------
// iR_DelayController Implementation
//------------------------------------------------------------------------
tresult PLUGIN_API iR_DelayController::initialize(FUnknown* context)
{
  // Here the Plug-in will be instanciated

  //---do not forget to call parent ------
  tresult result = EditControllerEx1::initialize(context);
  if (result != kResultOk) return result;
  setKnobMode(CKnobMode::kLinearMode);

  // Here you could register some parameters
  bypass_param = parameters.addParameter(
    STR16("Bypass"), nullptr, 1, 0,
    Vst::ParameterInfo::kCanAutomate /*| Vst::ParameterInfo::kIsBypass*/, kBypassID
  );

  // delay time
  bpm_sync_param = parameters.addParameter(
    STR16("BPM Sync"), nullptr, 1, 1,
    Vst::ParameterInfo::kCanAutomate, kEnableBPMSyncID
  );

  beat_select_L_param = parameters.addParameter(new Vst::StringListParameter(
    STR16("Beat Select L"), kBeatSelectLeftID, nullptr,
    Vst::ParameterInfo::kCanAutomate | Vst::ParameterInfo::kIsList
  ));

  beat_select_R_param = parameters.addParameter(new Vst::StringListParameter(
    STR16("Beat Select R"), kBeatSelectRightID, nullptr,
    Vst::ParameterInfo::kCanAutomate | Vst::ParameterInfo::kIsList
  ));
  for (auto&& param : { beat_select_L_param, beat_select_R_param }) {
    ((Vst::StringListParameter*)param)->appendString(STR16("1/64T")); // 0
    ((Vst::StringListParameter*)param)->appendString(STR16("1/64"));
    ((Vst::StringListParameter*)param)->appendString(STR16("1/32T"));
    ((Vst::StringListParameter*)param)->appendString(STR16("1/64D"));
    ((Vst::StringListParameter*)param)->appendString(STR16("1/32"));
    ((Vst::StringListParameter*)param)->appendString(STR16("1/16T"));
    ((Vst::StringListParameter*)param)->appendString(STR16("1/32D"));
    ((Vst::StringListParameter*)param)->appendString(STR16("1/16"));
    ((Vst::StringListParameter*)param)->appendString(STR16("1/8T"));
    ((Vst::StringListParameter*)param)->appendString(STR16("1/16D"));
    ((Vst::StringListParameter*)param)->appendString(STR16("1/8"));
    ((Vst::StringListParameter*)param)->appendString(STR16("1/4T"));
    ((Vst::StringListParameter*)param)->appendString(STR16("1/8D"));
    ((Vst::StringListParameter*)param)->appendString(STR16("1/4"));
    ((Vst::StringListParameter*)param)->appendString(STR16("1/2T"));
    ((Vst::StringListParameter*)param)->appendString(STR16("1/4D"));
    ((Vst::StringListParameter*)param)->appendString(STR16("1/2"));
    ((Vst::StringListParameter*)param)->appendString(STR16("1/2D"));
    ((Vst::StringListParameter*)param)->appendString(STR16("1Bar"));
    ((Vst::StringListParameter*)param)->appendString(STR16("2Bar"));
    ((Vst::StringListParameter*)param)->appendString(STR16("4Bar")); // 1
    param->setNormalized(0.66666f); // set to 1/4
  }

  delay_time_L_param = parameters.addParameter(new Vst::RangeParameter(
    STR16("Delay Time L"), kDelayTimeLeftID, STR16("s"),
    0.01f, 4.0f, 1.0f,
    0, Vst::ParameterInfo::kCanAutomate
  ));
  delay_time_L_param->setPrecision(2);

  delay_time_R_param = parameters.addParameter(new Vst::RangeParameter(
    STR16("Delay Time R"), kDelayTimeRightID, STR16("s"),
    0.01f, 4.0f, 1.0f,
    0, Vst::ParameterInfo::kCanAutomate
  ));
  delay_time_R_param->setPrecision(2);

  time_link_param = parameters.addParameter(
    STR16("Time Link"), nullptr, 1, 1,
    Vst::ParameterInfo::kCanAutomate, kEnableTimeLinkID
  );

  // feedback
  feedback_L_param = parameters.addParameter(new Vst::RangeParameter(
    STR16("Feedback L"), kFeedbackLeftID, STR16("%"),
    0.0f, 99.0f, 50.0f,
    0, Vst::ParameterInfo::kCanAutomate
  ));
  feedback_L_param->setPrecision(1);

  feedback_R_param = parameters.addParameter(new Vst::RangeParameter(
    STR16("Feedback R"), kFeedbackRightID, STR16("%"),
    0.0f, 99.0f, 50.0f,
    0, Vst::ParameterInfo::kCanAutomate
  ));
  feedback_R_param->setPrecision(1);

  feedback_link_param = parameters.addParameter(
    STR16("Feedback Link"), nullptr, 1, 1,
    Vst::ParameterInfo::kCanAutomate, kEnableFeedbackLinkID
  );

  // color
  dumping_param = parameters.addParameter(new Vst::RangeParameter(
    STR16("Dumping"), kDumpingID, STR16("%"),
    0.0f, 100.0f, 50.0f,
    0, Vst::ParameterInfo::kCanAutomate
  ));
  dumping_param->setPrecision(1);

  highcut_param = parameters.addParameter(new Vst::RangeParameter(
    STR16("High Cut"), kHighCutID, STR16("Hz"),
    1000.0f, 20000.0f, 1100.0f,
    0, Vst::ParameterInfo::kCanAutomate
  ));
  highcut_param->setPrecision(0);

  lowcut_param = parameters.addParameter(new Vst::RangeParameter(
    STR16("Low Cut"), kLowCutID, STR16("Hz"),
    20.0f, 1000.0f, 700.0f,
    0, Vst::ParameterInfo::kCanAutomate
  ));
  lowcut_param->setPrecision(0);

  // setting
  pingpong_param = parameters.addParameter(
    STR16("Enable Ping-Pong"), nullptr, 1, 0,
    Vst::ParameterInfo::kCanAutomate, kEnablePingPongID
  );

  swap_channel_param = parameters.addParameter(
    STR16("Swap Channel"), nullptr, 1, 0,
    Vst::ParameterInfo::kCanAutomate, kEnableSwapChannelID
  );

  // output
  stereo_width_param = parameters.addParameter(new Vst::RangeParameter(
    STR16("Stereo Width"), kStereoWidthID, STR16("%"),
    0.0f, 200.0f, 100.0f,
    0, Vst::ParameterInfo::kCanAutomate
  ));
  stereo_width_param->setPrecision(1);

  ducking_param = parameters.addParameter(new Vst::RangeParameter(
    STR16("Ducking"), kDuckingID, STR16("%"),
    0.0f, 100.0f, 0.0f,
    0, Vst::ParameterInfo::kCanAutomate
  ));
  ducking_param->setPrecision(1);

  mix_param = parameters.addParameter(new Vst::RangeParameter(
    STR16("Mix"), kMixID, STR16("%"),
    0.0f, 100.0f, 50.0f,
    0, Vst::ParameterInfo::kCanAutomate
  ));
  mix_param->setPrecision(1);

  output_gain_param = parameters.addParameter(new Vst::RangeParameter(
    STR16("Output Gain"), kOutputGainID, STR16("dB"),
    -6.0f, 6.0f, 0.0f,
    0, Vst::ParameterInfo::kCanAutomate
  ));
  output_gain_param->setPrecision(2);

  // other
  header_menu_param = parameters.addParameter(new Vst::StringListParameter(
    STR16("Header Menu"), kHeaderMenuID, nullptr,
    Vst::ParameterInfo::kIsList
  ));
  ((StringListParameter*)header_menu_param)->appendString(STR16("1"));
  ((StringListParameter*)header_menu_param)->appendString(STR16("2"));

  return result;
}

//------------------------------------------------------------------------
tresult PLUGIN_API iR_DelayController::terminate()
{
  // Here the Plug-in will be de-instanciated, last possibility to remove some memory!

  //---do not forget to call parent ------
  return EditControllerEx1::terminate();
}

//------------------------------------------------------------------------
tresult PLUGIN_API iR_DelayController::setComponentState(IBStream* state)
{
  if (!state) return kResultFalse;
  /**/
  bool saved_bypass = false;

  bool saved_bpm_sync = false;
  float saved_beat_L = 0.0f;
  float saved_beat_R = 0.0f;
  float saved_time_L = 0.0f;
  float saved_time_R = 0.0f;
  bool saved_time_link = false;

  float saved_feedback_L = 0.0f;
  float saved_feedback_R = 0.0f;
  bool saved_feedback_link = false;

  float saved_dumping = 0.0f;
  float saved_highcut = 0.0f;
  float saved_lowcut = 0.0f;

  bool saved_pingpong = false;
  bool saved_swap_channel = false;

  float saved_stereo_width = 0.0f;
  float saved_ducking = 0.0f;
  float saved_mix = 0.0f;
  float saved_output_gain = 0.0f;

  IBStreamer streamer(state, kLittleEndian);
  if (!streamer.readBool(saved_bypass)) return kResultFalse;
  setParamNormalized(kBypassID, saved_bypass);
  if (!streamer.readFloat(saved_output_gain)) return kResultFalse;
  setParamNormalized(kOutputGainID, saved_output_gain);

  if (!streamer.readBool(saved_bpm_sync)) return kResultFalse;
  if (!streamer.readFloat(saved_beat_L)) return kResultFalse;
  if (!streamer.readFloat(saved_beat_R)) return kResultFalse;
  if (!streamer.readFloat(saved_time_L)) return kResultFalse;
  if (!streamer.readFloat(saved_time_R)) return kResultFalse;
  if (!streamer.readBool(saved_time_link)) return kResultFalse;
  setParamNormalized(kEnableBPMSyncID, saved_bpm_sync);
  setParamNormalized(kBeatSelectLeftID, saved_beat_L);
  setParamNormalized(kBeatSelectRightID, saved_beat_R);
  setParamNormalized(kDelayTimeLeftID, saved_time_L);
  setParamNormalized(kDelayTimeRightID, saved_time_R);
  setParamNormalized(kEnableTimeLinkID, saved_time_link);

  if (!streamer.readFloat(saved_feedback_L)) return kResultFalse;
  if (!streamer.readFloat(saved_feedback_R)) return kResultFalse;
  if (!streamer.readBool(saved_feedback_link)) return kResultFalse;
  setParamNormalized(kFeedbackLeftID, saved_feedback_L);
  setParamNormalized(kFeedbackRightID, saved_feedback_R);
  setParamNormalized(kEnableFeedbackLinkID, saved_feedback_link);

  if (!streamer.readFloat(saved_dumping)) return kResultFalse;
  if (!streamer.readFloat(saved_highcut)) return kResultFalse;
  if (!streamer.readFloat(saved_lowcut)) return kResultFalse;
  setParamNormalized(kDumpingID, saved_dumping);
  setParamNormalized(kHighCutID, saved_highcut);
  setParamNormalized(kLowCutID, saved_lowcut);

  if (!streamer.readBool(saved_swap_channel)) return kResultFalse;
  if (!streamer.readBool(saved_pingpong)) return kResultFalse;
  setParamNormalized(kEnablePingPongID, saved_pingpong);
  setParamNormalized(kEnableSwapChannelID, saved_swap_channel);

  if (!streamer.readFloat(saved_stereo_width)) return kResultFalse;
  if (!streamer.readFloat(saved_ducking)) return kResultFalse;
  if (!streamer.readFloat(saved_mix)) return kResultFalse;
  
  setParamNormalized(kStereoWidthID, saved_stereo_width);
  setParamNormalized(kDuckingID, saved_ducking);
  setParamNormalized(kMixID, saved_mix);

  if (view) view->updateUI();
  /**/
  return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API iR_DelayController::setState(IBStream* state)
{
  // same as above setComponentState()
  return setComponentState(state);
}

//------------------------------------------------------------------------
tresult PLUGIN_API iR_DelayController::getState(IBStream* state)
{
  if (!state) return kResultFalse;
  /**/
  IBStreamer streamer(state, kLittleEndian);
  bool save_bypass = bypass_param->getNormalized();
  streamer.writeBool(save_bypass);

  bool save_bpm_sync = bpm_sync_param->getNormalized();
  float save_beat_L = beat_select_L_param->getNormalized();
  float save_beat_R = beat_select_R_param->getNormalized();
  float save_time_L = delay_time_L_param->getNormalized();
  float save_time_R = delay_time_R_param->getNormalized();
  bool save_time_link = time_link_param->getNormalized();
  streamer.writeBool(save_bpm_sync);
  streamer.writeFloat(save_beat_L);
  streamer.writeFloat(save_beat_R);
  streamer.writeFloat(save_time_L);
  streamer.writeFloat(save_time_R);
  streamer.writeBool(save_time_link);

  float save_feedback_L = feedback_L_param->getNormalized();
  float save_feedback_R = feedback_R_param->getNormalized();
  bool save_feedback_link = feedback_link_param->getNormalized();
  streamer.writeFloat(save_feedback_L);
  streamer.writeFloat(save_feedback_R);
  streamer.writeBool(save_feedback_link);

  float save_dumping = dumping_param->getNormalized();
  float save_highcut = highcut_param->getNormalized();
  float save_lowcut = lowcut_param->getNormalized();
  streamer.writeFloat(save_dumping);
  streamer.writeFloat(save_highcut);
  streamer.writeFloat(save_lowcut);

  bool save_pingpong = pingpong_param->getNormalized();
  bool save_swap_channel = swap_channel_param->getNormalized();
  streamer.writeBool(save_pingpong);
  streamer.writeBool(save_swap_channel);

  float save_stereo_width = stereo_width_param->getNormalized();
  float save_ducking = ducking_param->getNormalized();
  float save_mix = mix_param->getNormalized();
  float save_output_gain = output_gain_param->getNormalized();
  streamer.writeFloat(save_stereo_width);
  streamer.writeFloat(save_ducking);
  streamer.writeFloat(save_mix);
  streamer.writeFloat(save_output_gain);
  /**/
  return kResultOk;
}

//------------------------------------------------------------------------
IPlugView* PLUGIN_API iR_DelayController::createView(FIDString name)
{
  /**/
  if (FIDStringsEqual(name, Vst::ViewType::kEditor)) {
    view = new iR_DelayGUIEditor(this, 675, 410);
    return view;
  }
  /**/
  return nullptr;
}

//------------------------------------------------------------------------
tresult PLUGIN_API iR_DelayController::setParamNormalized(Vst::ParamID tag, Vst::ParamValue value)
{
  // called by host to update your parameters
  tresult result = EditControllerEx1::setParamNormalized(tag, value);
  return result;
}

//------------------------------------------------------------------------
tresult PLUGIN_API iR_DelayController::getParamStringByValue(Vst::ParamID tag, Vst::ParamValue valueNormalized, Vst::String128 string)
{
  // called by host to get a string for given normalized value of a specific parameter
  // (without having to set the value!)
  return EditControllerEx1::getParamStringByValue(tag, valueNormalized, string);
}

//------------------------------------------------------------------------
tresult PLUGIN_API iR_DelayController::getParamValueByString(Vst::ParamID tag, Vst::TChar* string, Vst::ParamValue& valueNormalized)
{
  // called by host to get a normalized value from a string representation of a specific parameter
  // (without having to set the value!)
  return EditControllerEx1::getParamValueByString(tag, string, valueNormalized);
}

/*
//------------------------------------------------------------------------
tresult PLUGIN_API iR_DelayController::notify(Vst::IMessage* message) {
  if (!strcmp(message->getMessageID(), u8"")) {
    const void* dataOut;
    uint32 size;
    message->getAttributes()->getBinary(u8"", dataOut, size);
  }
  return EditControllerEx1::notify(message);
}
*/

//------------------------------------------------------------------------
void iR_DelayController::releaseView()
{
  if (view) view->forget();
}

//------------------------------------------------------------------------
void iR_DelayController::setParamWithNormalize(Vst::ParamID tag, Vst::ParamValue value)
{
  setParamNormalized(tag, plainParamToNormalized(tag, value));
}

//------------------------------------------------------------------------
} // namespace INVOXRecords
