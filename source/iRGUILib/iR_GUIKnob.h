#ifndef IR_GUIKNOB_H
#define IR_GUIKNOB_H

#include "public.sdk/source/vst/vsteditcontroller.h"
#include "pluginterfaces/vst/vsttypes.h"
#include "vstgui/lib/cframe.h"
#include "vstgui/lib/cbitmap.h"
#include "vstgui/lib/controls/ccontrol.h"
#include "vstgui/lib/controls/cknob.h"
#include "vstgui/lib/controls/ctextlabel.h"
#include "vstgui/lib/controls/ctextedit.h"

#include <functional>

using namespace Steinberg::Vst;
using namespace VSTGUI;

namespace INVOXRecords {
//----------------------------------------------------------------------------------------------------------------------
// ControlKnob class
//----------------------------------------------------------------------------------------------------------------------
class ControlKnob : public CAnimKnob
{
public:
  ControlKnob(const CRect& size, IControlListener* listener, int32_t tag, CBitmap* background, UTF8StringPtr title);
  
  void setAlwaysShowValueLabel(bool state) { always_show_value_label_ = state; }
  void setUsableState(bool usable);
  void setVisible(bool state) override;
  void editValueProcess();

  bool getAlwaysShowValueLabel() const { return always_show_value_label_; }
  CTextLabel* getTitleLabelPtr() const { return title_label_; }
  CTextLabel* getValueLabelPtr() const { return value_label_; }
private:
  CTextLabel* title_label_ = nullptr;
  CTextLabel* value_label_ = nullptr;

  bool always_show_value_label_ = false;
};

//----------------------------------------------------------------------------------------------------------------------
// ControlKnob implementation
//----------------------------------------------------------------------------------------------------------------------
inline ControlKnob::ControlKnob(const CRect& size, IControlListener* listener, int32_t tag, CBitmap* background, UTF8StringPtr title)
  : CAnimKnob(size, listener, tag, background)
  , title_label_(new CTextLabel(CRect(size.left - 10, size.top - 20, size.right + 10, size.top), title))
  , value_label_(new CTextLabel(CRect(size.left, size.top + 42, size.right, size.bottom)))
{
  const CRect mousable_area(0, 0, 0, 0);
  title_label_->setMouseableArea(mousable_area);
  title_label_->setFont(kNormalFont);
  title_label_->setFontColor(kWhiteCColor);
  title_label_->setBackColor(kTransparentCColor);
  title_label_->setFrameColor(kTransparentCColor);

  value_label_->setMouseableArea(mousable_area);
  value_label_->setFont(kNormalFont);
  value_label_->setFontColor(kWhiteCColor);
  value_label_->setBackColor(CColor(0, 0, 0, 150.0f));
  value_label_->setFrameColor(kTransparentCColor);
}

inline void ControlKnob::setUsableState(bool usable)
{
  if (usable) {
    setAlphaValue(1.0f);
    title_label_->setAlphaValue(1.0f);
    if (always_show_value_label_) 
      value_label_->setAlphaValue(1.0f);
    setMouseEnabled(true);
  } else {
    setAlphaValue(0.5f);
    title_label_->setAlphaValue(0.5f);
    if (always_show_value_label_) 
      value_label_->setAlphaValue(0.5f);
    setMouseEnabled(false);
  }
}

inline void ControlKnob::setVisible(bool state)
{
  CView::setVisible(state);
  title_label_->setVisible(state);
  value_label_->setVisible(state);
}

inline void ControlKnob::editValueProcess()
{
  value_label_->setAlphaValue(always_show_value_label_ ? 1.0f : isEditing());
}

//----------------------------------------------------------------------------------------------------------------------
// Enums
//----------------------------------------------------------------------------------------------------------------------
enum class KnobStartPoint
{
  kCenter, kLeft, kRight
};

} // namespace INVOXRecords
#endif // IR_GUIKNOB_H
