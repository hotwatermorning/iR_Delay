//------------------------------------------------------------------------
// Copyright(c) 2021 iNVOX Records.
//------------------------------------------------------------------------

#ifndef GUIEDITOR_H
#define GUIEDITOR_H

#include "iRGUILib/iR_GUIBase.h"
#include "iRGUILib/iR_GUIField.h"

#include "public.sdk/source/vst/vstguieditor.h"
#include "pluginterfaces/vst/ivstplugview.h"

using namespace Steinberg;
using namespace VSTGUI;

namespace INVOXRecords {

class iR_DelayGUIEditor : public iRGUIBase
{
 public:
	iR_DelayGUIEditor(void* controller, int FrameWidth, int FrameHeight);
	virtual bool PLUGIN_API open(void* parent, const PlatformType& platformType = kDefaultNative);
	virtual void PLUGIN_API close();
	virtual CMessageResult notify(CBaseObject* sender, IdStringPtr message);

	void valueChanged(CControl* pControl);
	void updateValueProcess(Vst::ParamID index, Vst::ParamValue value) override;

	DELEGATE_REFCOUNT(VSTGUIEditor)

 protected:
	 CVSTGUITimer* timer = nullptr;

	 Header* header = nullptr;

	 // delay time (4)
	 CategoryField* field_delay_time = nullptr;
	 ControlSwitch* switch_bpm_sync = nullptr;
	 ControlMenu* menu_beat_select_L = nullptr;
	 ControlMenu* menu_beat_select_R = nullptr;
	 ControlKnob* knob_delay_time_L = nullptr;
	 ControlKnob* knob_delay_time_R = nullptr;
	 ControlSwitch* switch_time_link = nullptr;

	 // setting (2)
	 CategoryField* field_setting = nullptr;
	 ControlSwitch* switch_pingpong = nullptr;
	 ControlSwitch* switch_swap_channel = nullptr;

	 // feedback (3)
	 CategoryField* field_feedback = nullptr;
	 ControlKnob* knob_feedback_left = nullptr;
	 ControlKnob* knob_feedback_right = nullptr;
	 ControlSwitch* switch_feedback_link = nullptr;

	 // color (3)
	 CategoryField* field_color = nullptr;
	 ControlKnob* knob_dumping = nullptr;
	 ControlKnob* knob_highcut = nullptr;
	 ControlKnob* knob_lowcut = nullptr;

	 // output (3)
	 CategoryField* field_output = nullptr;
	 ControlKnob* knob_stereowidth = nullptr;
	 ControlKnob* knob_ducking = nullptr;
	 ControlKnob* knob_mix = nullptr;
	 ControlKnob* knob_output_gain = nullptr;
};
} // namespace INVOXRecords
#endif
