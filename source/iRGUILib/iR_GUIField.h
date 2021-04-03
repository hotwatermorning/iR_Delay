#ifndef IR_GUIFIELD_H
#define IR_GUIFIELD_H

#include "vstgui/lib/cview.h"
#include "vstgui/lib/cdrawcontext.h"

using namespace VSTGUI;

namespace INVOXRecords {
//----------------------------------------------------------------------------------------------------------------------
// CategoryField class
//----------------------------------------------------------------------------------------------------------------------
class CategoryField : public CView
{
public:
  CategoryField(const CRect size, UTF8StringPtr title, CColor color = CColor(200, 200, 200));
  void draw(CDrawContext* pContext) override;

  CTextLabel* getTitleLabelPtr() const { return title_label_; }
private:
  CTextLabel* title_label_ = nullptr;
  CColor color_;
};

//----------------------------------------------------------------------------------------------------------------------
// CategoryField implementation
//----------------------------------------------------------------------------------------------------------------------
inline CategoryField::CategoryField(const CRect size, UTF8StringPtr title, CColor color)
  : CView(size)
  , title_label_(new CTextLabel(CRect(size.left + 20, size.top - 10, size.right, size.top + 13), title))
  , color_(color)
{
  title_label_->setFont(kNormalFontVeryBig);
  title_label_->setFontColor(color);
  title_label_->setBackColor(kTransparentCColor);
  title_label_->setFrameColor(kTransparentCColor);
  title_label_->sizeToFit();
}

inline void CategoryField::draw(CDrawContext* pContext)
{
  const CRect size = getViewSize();
  const CPoint top_left(size.left + 1, size.top + 1);
  const CPoint top_right(size.right - 1, size.top + 1);
  const CPoint bottom_left(size.left + 1, size.bottom - 1);
  const CPoint bottom_right(size.right - 1, size.bottom - 1);

  pContext->setFrameColor(color_);
  pContext->setDrawMode(kAntiAliasing);
  pContext->setLineWidth(2);

  // left line
  pContext->drawLine(bottom_left + CPoint(0, -15), top_left + CPoint(0, 15));
  // top-left arc
  pContext->drawArc(CRect(top_left, CPoint(30, 30)), 180.0f, 270.0f);
  // top line
  // arc + margin + text_width + margin
  float top_line_left = (size.left + 15) + 5 +title_label_->getViewSize().getWidth() + 5;
  if (top_line_left < top_right.x - 15)
    pContext->drawLine(top_left + CPoint(title_label_->getViewSize().getWidth() + 25, 0), top_right + CPoint(-15, 0));
  // top-right arc
  pContext->drawArc(CRect(top_right + CPoint(-30, 0), CPoint(30, 30)), 270.0f, 360.0f);
  // right line
  pContext->drawLine(top_right + CPoint(0, 15), bottom_right + CPoint(0, -15));
  // bottom-right arc
  pContext->drawArc(CRect(bottom_right + CPoint(-30, -30), CPoint(30, 30)), 0.0f, 90.0f);
  // bottom line
  pContext->drawLine(bottom_right + CPoint(-15, 0), bottom_left + CPoint(15, 0));
  // bottom-left arc
  pContext->drawArc(CRect(bottom_left + CPoint(0, -30), CPoint(30, 30)), 90.0f, 180.0f);

  setDirty();
}

} // namespace INVOXRecords
#endif // IR_GUIFIELD_H