#ifndef IR_GUISWITCH_H
#define IR_GUISWITCH_H

#include "public.sdk/source/vst/vsteditcontroller.h"
#include "pluginterfaces/vst/vsttypes.h"
#include "vstgui/lib/cframe.h"
#include "vstgui/lib/cbitmap.h"
#include "vstgui/lib/controls/ccontrol.h"
#include "vstgui/lib/controls/cbuttons.h"
#include "vstgui/lib/controls/ctextlabel.h"

using namespace Steinberg::Vst;
using namespace VSTGUI;

namespace INVOXRecords {
//----------------------------------------------------------------------------------------------------------------------
// ControlSwitch class
//----------------------------------------------------------------------------------------------------------------------
class ControlSwitch : public COnOffButton
{
public:
  ControlSwitch(const CRect& size, IControlListener* listener, int32_t tag, CBitmap* background, UTF8StringPtr title);

  // gui update method
  void setUsableState(bool usable = true);

  CTextLabel* getTitleLabelPtr() const { return title_label; }
private:
  CTextLabel* title_label = nullptr;
};

//----------------------------------------------------------------------------------------------------------------------
// ControlSwitch implementation
//----------------------------------------------------------------------------------------------------------------------
inline ControlSwitch::ControlSwitch(const CRect& size, IControlListener* listener, int32_t tag, CBitmap* background, UTF8StringPtr title)
  : COnOffButton(size, listener, tag, background)
  , title_label(new CTextLabel(CRect(size.left - 10, size.top - 20, size.right + 10, size.top), title))
{
  const CRect mousable_area(0, 0, 0, 0);
  title_label->setMouseableArea(mousable_area);
  title_label->setFont(kNormalFont);
  title_label->setFontColor(kWhiteCColor);
  title_label->setBackColor(kTransparentCColor);
  title_label->setFrameColor(kTransparentCColor);
}

inline void ControlSwitch::setUsableState(bool usable)
{
  if (usable) {
    setAlphaValue(1.0f);
    title_label->setAlphaValue(1.0f);
    setMouseEnabled(true);
  }
  else {
    setAlphaValue(0.5f);
    title_label->setAlphaValue(0.5f);
    setMouseEnabled(false);
  }
}
}
#endif // !IR_GUIKNOB
