//------------------------------------------------------------------------
// Copyright(c) 2021 iNVOX Records.
//------------------------------------------------------------------------

#pragma once

#include "iR_Delay_guieditor.h"
#include "public.sdk/source/vst/vsteditcontroller.h"

using namespace Steinberg;

namespace INVOXRecords {
//------------------------------------------------------------------------
//  iR_DelayController
//------------------------------------------------------------------------
class iR_DelayController : public Vst::EditControllerEx1
{
 public:
  iR_DelayController() = default;
  ~iR_DelayController() SMTG_OVERRIDE = default;

  static FUnknown* createInstance(void* /*context*/)
  {
    return (Vst::IEditController*)new iR_DelayController;
  }

  // IPluginBase
  tresult PLUGIN_API initialize(FUnknown* context) SMTG_OVERRIDE;
  tresult PLUGIN_API terminate() SMTG_OVERRIDE;

  // EditController
  tresult PLUGIN_API setComponentState(IBStream* state) SMTG_OVERRIDE;
  IPlugView* PLUGIN_API createView(FIDString name) SMTG_OVERRIDE;
  tresult PLUGIN_API setState(IBStream* state) SMTG_OVERRIDE;
  tresult PLUGIN_API getState(IBStream* state) SMTG_OVERRIDE;
  tresult PLUGIN_API setParamNormalized(Vst::ParamID tag,
    Vst::ParamValue value) SMTG_OVERRIDE;
  tresult PLUGIN_API getParamStringByValue(Vst::ParamID tag,
    Vst::ParamValue valueNormalized,
    Vst::String128 string) SMTG_OVERRIDE;
  tresult PLUGIN_API getParamValueByString(Vst::ParamID tag,
    Vst::TChar* string,
    Vst::ParamValue& valueNormalized) SMTG_OVERRIDE;

  void releaseView();
  void setParamWithNormalize(Vst::ParamID tag, Vst::ParamValue value);
  //tresult PLUGIN_API notify(Vst::IMessage* message) SMTG_OVERRIDE;

  //---Interface---------
  DEFINE_INTERFACES
    // Here you can add more supported VST3 interfaces
    // DEF_INTERFACE (Vst::IXXX)
    END_DEFINE_INTERFACES(EditController)
    DELEGATE_REFCOUNT(EditController)

//------------------------------------------------------------------------
 protected:
  iR_DelayGUIEditor* view = nullptr;

  Vst::Parameter* bypass_param = nullptr;

  // delay time
  Vst::Parameter* bpm_sync_param = nullptr;
  Vst::Parameter* beat_select_L_param = nullptr;
  Vst::Parameter* beat_select_R_param = nullptr;
  Vst::Parameter* delay_time_L_param = nullptr;
  Vst::Parameter* delay_time_R_param = nullptr;
  Vst::Parameter* time_link_param = nullptr;

  // feedback
  Vst::Parameter* feedback_L_param = nullptr;
  Vst::Parameter* feedback_R_param = nullptr;
  Vst::Parameter* feedback_link_param = nullptr;

  // color
  Vst::Parameter* dumping_param = nullptr;
  Vst::Parameter* highcut_param = nullptr;
  Vst::Parameter* lowcut_param = nullptr;

  // setting
  Vst::Parameter* swap_channel_param = nullptr;
  Vst::Parameter* pingpong_param = nullptr;

  // output
  Vst::Parameter* stereo_width_param = nullptr;
  Vst::Parameter* mix_param = nullptr;
  Vst::Parameter* ducking_param = nullptr;
  Vst::Parameter* output_gain_param = nullptr;

  // other
  Vst::Parameter* header_menu_param = nullptr;
};
//------------------------------------------------------------------------
} // namespace INVOXRecords
