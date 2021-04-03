//------------------------------------------------------------------------
// Copyright(c) 2021 iNVOX Records.
//------------------------------------------------------------------------

#include "iR_Delay_paramids.h"
#include "iR_Delay_guieditor.h"
#include "iR_Delay_controller.h"

namespace INVOXRecords {
//------------------------------------------------------------------------
// iR_DelayGUIEditor
//------------------------------------------------------------------------
iR_DelayGUIEditor::iR_DelayGUIEditor(void* controller, int FrameWidth, int FrameHeight)
  : iRGUIBase(controller, FrameWidth, FrameHeight) 
{
}

//------------------------------------------------------------------------
bool PLUGIN_API iR_DelayGUIEditor::open(void* parent, const PlatformType& platformType)
{
  bool res = iRGUIBase::open(parent, platformType);
  if (!res) return false;
  if (!timer) timer = new CVSTGUITimer(this, 20, true);
  frame->enableTooltips(true, 500U);

  // header
  header = createHeader(u8"iR Delay", kHeaderMenuID);

  // delay time
  field_delay_time = createField(CRect(10, 55, 415, 155), u8"Delay Time");
  menu_beat_select_L = createMenu(kBeatSelectLeftID, 20, 65, u8"Beat L", MenuSize::kShort);
  menu_beat_select_R = createMenu(kBeatSelectRightID, 120, 65, u8"Beat R", MenuSize::kShort);
  for (auto&& menu : { menu_beat_select_L, menu_beat_select_R }) {
    menu->addEntry(u8"1/64T");
    menu->addEntry(u8"1/64");
    menu->addEntry(u8"1/32T");
    menu->addEntry(u8"1/64D");
    menu->addEntry(u8"1/32");
    menu->addEntry(u8"1/16T");
    menu->addEntry(u8"1/32D");
    menu->addEntry(u8"1/16");
    menu->addEntry(u8"1/8T");
    menu->addEntry(u8"1/16D");
    menu->addEntry(u8"1/8");
    menu->addEntry(u8"1/4T");
    menu->addEntry(u8"1/8D");
    menu->addEntry(u8"1/4");
    menu->addEntry(u8"1/2T");
    menu->addEntry(u8"1/4D");
    menu->addEntry(u8"1/2");
    menu->addEntry(u8"1/2D");
    menu->addEntry(u8"1Bar");
    menu->addEntry(u8"2Bar");
    menu->addEntry(u8"4Bar");
  }
  knob_delay_time_L = createKnob(kDelayTimeLeftID, 20, 65, u8"Time L", KnobStartPoint::kLeft);
  knob_delay_time_R = createKnob(kDelayTimeRightID, 120, 65, u8"Time R", KnobStartPoint::kLeft);
  switch_bpm_sync = createSwitch(kEnableBPMSyncID, 220, 65, u8"BPM Sync");
  switch_time_link = createSwitch(kEnableTimeLinkID, 320, 65, u8"Time Link");

  // setting
  field_setting = createField(CRect(460, 55, 665, 155), u8"Setting");
  switch_pingpong = createSwitch(kEnablePingPongID, 470, 65, u8"Ping-Pong");
  switch_swap_channel = createSwitch(kEnableSwapChannelID, 570, 65, u8"Swap Channel");

  // feedback
  field_feedback = createField(CRect(10, 175, 315, 275), u8"Feedback");
  knob_feedback_left = createKnob(kFeedbackLeftID, 20, 185, u8"Feedback L", KnobStartPoint::kLeft);
  knob_feedback_right = createKnob(kFeedbackRightID, 120, 185, u8"Feedback R", KnobStartPoint::kLeft);
  switch_feedback_link = createSwitch(kEnableFeedbackLinkID, 220, 185, u8"Feedback Link");

  // color
  field_color = createField(CRect(360, 175, 665, 275), u8"Color");
  knob_dumping = createKnob(kDumpingID, 370, 185, u8"Dumping", KnobStartPoint::kLeft);
  knob_highcut = createKnob(kHighCutID, 570, 185, u8"High Cut", KnobStartPoint::kRight);
  knob_lowcut = createKnob(kLowCutID, 470, 185, u8"Low Cut", KnobStartPoint::kLeft);

  // output
  field_output = createField(CRect(10, 295, 415, 395), u8"Output");
  knob_stereowidth = createKnob(kStereoWidthID, 20, 305, u8"Stereo Width", KnobStartPoint::kLeft);
  knob_ducking = createKnob(kDuckingID, 120, 305, u8"Ducking", KnobStartPoint::kLeft);
  knob_mix = createKnob(kMixID, 220, 305, u8"Mix", KnobStartPoint::kLeft);
  knob_output_gain = createKnob(kOutputGainID, 320, 305, u8"Output Gain", KnobStartPoint::kCenter);

  // set default value if default is not 0.5
  knob_delay_time_L->setDefaultValue(0.25f);
  knob_delay_time_R->setDefaultValue(0.25f);
  knob_highcut->setDefaultValue(0.25f);
  knob_ducking->setDefaultValue(0.0f);
  
  // set tool tips text
  menu_beat_select_L->setTooltipText(u8"When BPM Sync is enabled, the feedback period of the left channel delay signal can be specified.");
  menu_beat_select_R->setTooltipText(u8"When BPM Sync is enabled, the feedback period of the right channel delay signal can be specified.");
  knob_delay_time_L->setTooltipText(u8"Specifies the left channel feedback period delay signal.");
  knob_delay_time_R->setTooltipText(u8"Specifies the right channel feedback period delay signal.");
  switch_bpm_sync->setTooltipText(u8"When enabled, the feedback period of the delay signal will be synchronized with the BPM.");
  switch_time_link->setTooltipText(u8"When enabled, the right channel feedback period depends on the left channel feedback period.");

  switch_pingpong->setTooltipText(u8"When enabled, it produces a \"ping - pong effect\"\nin which the delay signals\nof the left and right channels feed back alternately.");
  switch_swap_channel->setTooltipText(u8"When Ping-Pong is enabled, it swaps the output of the delay signals of the left and right channels.");

  knob_feedback_left->setTooltipText(u8"Specifies the percentage of the left channel input signal to be fed back.");
  knob_feedback_right->setTooltipText(u8"Specifies the percentage of the right channel input signal to be fed back.");
  switch_feedback_link->setTooltipText(u8"When enabled, the right channel feedback depends on the left channel feedback.");

  knob_dumping->setTooltipText(u8"Specifies how much the damping filter affects the output per feedback.");
  knob_highcut->setTooltipText(u8"Cuts off the high frequency range of the delay signal.");
  knob_lowcut->setTooltipText(u8"Cuts off the low frequency range of the delay signal.");

  knob_stereowidth->setTooltipText(u8"The higher this value is, the greater the spread of sound from left to right.");
  knob_ducking->setTooltipText(u8"The higher this value is, the louder the delay signal will be suppressed\nwhen the input signal is loud.");
  knob_mix->setTooltipText(u8"Specifies the ratio between the input signal and the delay signal.\n0%: only the input signal, 100%: only the delay signal.");
  knob_output_gain->setTooltipText(u8"Adjusts the volume of the output signal.");

  updateUI();
  return true;
}

//------------------------------------------------------------------------
void PLUGIN_API iR_DelayGUIEditor::close()
{
  iRGUIBase::close();
  if (timer) timer->forget();
}


//------------------------------------------------------------------------
CMessageResult iR_DelayGUIEditor::notify(CBaseObject* sender, const char* message)
{
  if (message == CVSTGUITimer::kMsgTimer && sender) {
    if (frame) {
      frame->idle();
      updateUI();
    }
    return kMessageNotified;
  }
  return kMessageUnknown;
}


//------------------------------------------------------------------------
void iR_DelayGUIEditor::valueChanged(CControl* pControl)
{
  Vst::ParamID index = pControl->getTag();
  Vst::ParamValue value = pControl->getValueNormalized();
  controller->setParamNormalized(index, value);
  updateValueProcess(index, value);
  controller->performEdit(index, value);
}

//------------------------------------------------------------------------
void iR_DelayGUIEditor::updateValueProcess(Vst::ParamID index, Vst::ParamValue value)
{
  switch (index) {
  case kEnableBPMSyncID:
    menu_beat_select_L->setVisible(value > 0.5f);
    menu_beat_select_R->setVisible(value > 0.5f);
    knob_delay_time_L->setVisible(value < 0.5f);
    knob_delay_time_R->setVisible(value < 0.5f);
    break;
  case kDelayTimeLeftID:
    knob_delay_time_L->editValueProcess();
    setTextLabel(index, value, knob_delay_time_L->getValueLabelPtr(), nullptr, u8" s");
    break;
  case kDelayTimeRightID:
    knob_delay_time_R->editValueProcess();
    setTextLabel(index, value, knob_delay_time_R->getValueLabelPtr(), nullptr, u8" s");
    break;
  case kEnableTimeLinkID:
    knob_delay_time_R->setUsableState(value < 0.5f);
    menu_beat_select_R->setUsableState(value < 0.5f);
    break;
  case kFeedbackLeftID:
    knob_feedback_left->editValueProcess();
    setTextLabel(index, value, knob_feedback_left->getValueLabelPtr(), nullptr, u8" %");
    break;
  case kFeedbackRightID:
    knob_feedback_right->editValueProcess();
    setTextLabel(index, value, knob_feedback_right->getValueLabelPtr(), nullptr, u8" %");
    break;
  case kEnableFeedbackLinkID:
    knob_feedback_right->setUsableState(value < 0.5f);
    break;
  case kDumpingID:
    knob_dumping->editValueProcess();
    setTextLabel(index, value, knob_dumping->getValueLabelPtr(), nullptr, u8" %");
    break;
  case kHighCutID:
    knob_highcut->editValueProcess();
    setTextLabel(index, value, knob_highcut->getValueLabelPtr(), nullptr, u8" Hz");
    break;
  case kLowCutID:
    knob_lowcut->editValueProcess();
    setTextLabel(index, value, knob_lowcut->getValueLabelPtr(), nullptr, u8" Hz");
    break;
  case kStereoWidthID:
    knob_stereowidth->editValueProcess();
    setTextLabel(index, value, knob_stereowidth->getValueLabelPtr(), nullptr, u8" %");
    break;
  case kDuckingID:
    knob_ducking->editValueProcess();
    setTextLabel(index, value, knob_ducking->getValueLabelPtr(), nullptr, u8" %");
    break;
  case kMixID:
    knob_mix->editValueProcess();
    setTextLabel(index, value, knob_mix->getValueLabelPtr(), nullptr, u8" %");
    break;
  case kOutputGainID:
    knob_output_gain->editValueProcess();
    setTextLabel(index, value, knob_output_gain->getValueLabelPtr(), nullptr, u8" dB");
    break;
  case kHeaderMenuID:
    bool checked_0 = header->getHeaderMenu()->getEntry(0)->isChecked(); // always show knob value
    bool checked_1 = header->getHeaderMenu()->getEntry(1)->isChecked(); // parameter tips
    setAlwaysShowAllKnobValue(checked_0);
    frame->enableTooltips(checked_1, 500U);
    break;
  }
}
//------------------------------------------------------------------------
} // namespace INVOXRecords
