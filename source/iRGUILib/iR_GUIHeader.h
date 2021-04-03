#ifndef IR_GUIHEADER_H
#define IR_GUIHEADER_H

#include "iR_GUIMenu.h"

#include "vstgui/lib/cgradientview.h"
#include "vstgui/lib/cgradient.h"
#include "vstgui/lib/controls/ctextlabel.h"
#include "vstgui/lib/cbitmap.h"
#include "vstgui/lib/cdrawcontext.h"

using namespace VSTGUI;

namespace INVOXRecords {
//----------------------------------------------------------------------------------------------------------------------
// Header class
//----------------------------------------------------------------------------------------------------------------------
class Header : public CGradientView
{
public:
  Header(const CRect size, UTF8StringPtr title);

  void setHeaderMenu(ControlMenu* header_menu) { header_menu_ = header_menu; }

  ControlMenu* getHeaderMenu() const { return header_menu_; }
  CTextLabel* getTitleLabelPtr() const { return title_label_; }
  CTextLabel* getCopyrightLabelPtr() const { return copyright_label_; }
private:
  ControlMenu* header_menu_ = nullptr;
  CTextLabel* title_label_ = nullptr;
  CTextLabel* copyright_label_ = nullptr;
};

//----------------------------------------------------------------------------------------------------------------------
// Header implementation
//----------------------------------------------------------------------------------------------------------------------
inline Header::Header(const CRect size, UTF8StringPtr title)
  : CGradientView(size)
  , title_label_(new CTextLabel(CRect(10, size.top, 200, size.bottom), title))
  , copyright_label_(new CTextLabel(CRect(size.right - 150, size.bottom - 25, size.right - 40, size.bottom), u8"©iNVOX Records"))
{
  setRoundRectRadius(0.f);
  setFrameWidth(0);
  title_label_->setFrameColor(kTransparentCColor);
  title_label_->setBackColor(kTransparentCColor);
  title_label_->setHoriAlign(CHoriTxtAlign::kLeftText);
  title_label_->setFontColor(CColor(255, 176, 57));
  copyright_label_->setFrameColor(kTransparentCColor);
  copyright_label_->setBackColor(kTransparentCColor);
  copyright_label_->setHoriAlign(CHoriTxtAlign::kRightText);
  copyright_label_->setFontColor(CColor(200, 200, 200));

  CColor top_color(60, 60, 60);
  CColor bottom_color(50, 50, 50);
  CGradient* gradient = CGradient::create(0.0, 1.0, top_color, bottom_color);
  setGradient(gradient);
}

} // namespace INVOXRecords 
#endif // IR_GUIHEADER_H
