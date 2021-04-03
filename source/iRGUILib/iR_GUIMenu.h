#ifndef IR_GUIMENU_H
#define IR_GUIMENU_H

#include "public.sdk/source/vst/vsteditcontroller.h"
#include "pluginterfaces/vst/vsttypes.h"
#include "vstgui/lib/cframe.h"
#include "vstgui/lib/cbitmap.h"
#include "vstgui/lib/controls/ccontrol.h"
#include "vstgui/lib/controls/coptionmenu.h"
#include "vstgui/lib/controls/ctextlabel.h"

using namespace Steinberg::Vst;
using namespace VSTGUI;

namespace INVOXRecords {
//----------------------------------------------------------------------------------------------------------------------
// ControlMenu class
//----------------------------------------------------------------------------------------------------------------------
class ControlMenu : public COptionMenu
{
public:
  ControlMenu(const CRect& size, IControlListener* listener, int32_t tag, CBitmap* background, CBitmap* background_click, UTF8StringPtr title);

  // gui update method
  void setUsableState(bool usable = true);
  void setVisible(bool state) override;

  CTextLabel* getTitleLabelPtr() const { return title_label; }
private:
  CTextLabel* title_label = nullptr;
};

//----------------------------------------------------------------------------------------------------------------------
// ControlMenu implementation
//----------------------------------------------------------------------------------------------------------------------
inline ControlMenu::ControlMenu(const CRect& size, IControlListener* listener, int32_t tag, CBitmap* background, CBitmap* background_click, UTF8StringPtr title)
  : COptionMenu(size, listener, tag, background, background_click)
  , title_label(new CTextLabel(CRect(size.left - 10, size.top - 46, size.right + 10, size.top), title))
{
  const CRect mousable_area(0, 0, 0, 0);
  title_label->setMouseableArea(mousable_area);
  title_label->setFont(kNormalFont);
  title_label->setFontColor(kWhiteCColor);
  title_label->setBackColor(kTransparentCColor);
  title_label->setFrameColor(kTransparentCColor);
}

inline void ControlMenu::setUsableState(bool usable)
{
  if (usable) {
    setAlphaValue(1.0f);
    title_label->setAlphaValue(1.0f);
    setMouseEnabled(true);
  } else {
    setAlphaValue(0.5f);
    title_label->setAlphaValue(0.5f);
    setMouseEnabled(false);
  }
}

inline void ControlMenu::setVisible(bool state)
{
  CView::setVisible(state);
  title_label->setVisible(state);
}

//----------------------------------------------------------------------------------------------------------------------
// Enums
//----------------------------------------------------------------------------------------------------------------------
enum class MenuSize
{
  kShort, kMidium, kLong
};
}
#endif // !IR_GUIMENU_H
