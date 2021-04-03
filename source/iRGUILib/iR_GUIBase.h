//------------------------------------------------------------------------
// Copyright(c) 2021 iNVOX Records.
//------------------------------------------------------------------------

#ifndef IR_GUIBASE_H
#define IR_GUIBASE_H

#include "iR_GUIKnob.h"
#include "iR_GUISwitch.h"
#include "iR_GUIMenu.h"
#include "iR_GUIField.h"
#include "iR_GUIHeader.h"

#include "public.sdk/source/vst/vstguieditor.h"
#include "pluginterfaces/vst/ivstplugview.h"

#include <vector>

using namespace Steinberg;
using namespace VSTGUI;

namespace INVOXRecords {
//------------------------------------------------------------------------
// iRGUIBase
//------------------------------------------------------------------------
class iRGUIBase : public Vst::VSTGUIEditor, public IControlListener
{
//------------------------------------------------------------------------
 public:
  iRGUIBase(void* controller, int FrameWidth, int FrameHeight);

  virtual bool PLUGIN_API open(void* parent, const PlatformType& platformType = kDefaultNative);
  virtual void PLUGIN_API close();

  void updateUI();
  virtual void updateValueProcess(Vst::ParamID index, Vst::ParamValue value) = 0;

  COnOffButton* createOnOffButton(Vst::ParamID tag, int x, int y, const CResourceDescription& filename);
  CVerticalSlider* createVerticalSlider(Vst::ParamID tag, int x, int y, const CResourceDescription& background, const CResourceDescription& handle);
  CKickButton* createKickButton(Vst::ParamID tag, int x, int y, const CResourceDescription& filename);
  CTextButton* createTextButton(Vst::ParamID tag, int x, int y, int width, int height, UTF8StringPtr title);
  CCheckBox* createCheckBox(Vst::ParamID tag, int x, int y, UTF8StringPtr title, const CResourceDescription& filename);
  ControlKnob* createKnob(Vst::ParamID tag, int x, int y, UTF8StringPtr title, KnobStartPoint start_point);
  ControlSwitch* createSwitch(Vst::ParamID tag, int x, int y, UTF8StringPtr title);
  ControlMenu* createMenu(Vst::ParamID tag, int x, int y, UTF8StringPtr title, MenuSize menu_size);
  CategoryField* createField(CRect size, UTF8StringPtr title, CColor color = CColor(200, 200, 200));
  Header* createHeader(UTF8StringPtr title, Vst::ParamID header_menu_tag);
  ControlMenu* createHeaderMenu(Vst::ParamID tag);
  CTextLabel* createTextLabel(int x, int y, int width, int height, UTF8StringPtr text);
  CTextEdit* createTextEdit(Vst::ParamID tag, int x, int y, int width, int height, UTF8StringPtr text);
  void setTextLabel(Vst::ParamID index, Vst::ParamValue value, CControl* control, UTF8StringPtr prefix, UTF8StringPtr suffix);
  void setAlwaysShowAllKnobValue(bool state);
  float stringToValue(CTextEdit* edit, float min, float max);

  DELEGATE_REFCOUNT(VSTGUIEditor)

//------------------------------------------------------------------------
 protected:
  //initial frame size
  int FrameWidth = 300;
  int FrameHeight = 300;

private:
  std::vector<CControl*> control_container;
  std::vector<ControlKnob*> knob_container;
};

#if MAC
#if TARGET_OS_IPHONE
static CFontDesc gPluginTitleFont("ArialMT", 24);
#else
static CFontDesc gPluginTitleFont("Arial", 24);
#endif

#elif WINDOWS
static CFontDesc gPluginTitleFont("Arial", 24);

#else
static CFontDesc gPluginTitleFont("Arial", 24);

#endif

const CFontRef kPluginTitleFont = &gPluginTitleFont;
//------------------------------------------------------------------------
} // namespace INVOXRecords
#endif
