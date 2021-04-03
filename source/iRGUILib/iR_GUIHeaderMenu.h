#ifndef IR_GUIHEADERMENU_H
#define IR_GUIHEADERMENU_H

#include "vstgui/lib/controls/coptionmenu.h"

using namespace VSTGUI;

namespace INVOXRecords {
//----------------------------------------------------------------------------------------------------------------------
// HeaderMenu class
//----------------------------------------------------------------------------------------------------------------------
class HeaderMenu : public COptionMenu
{
public:
  HeaderMenu(const CRect& size, IControlListener* listener, int32_t tag, CBitmap* background, CBitmap* background_click);
};

//----------------------------------------------------------------------------------------------------------------------
// HeaderMenu implementation
//----------------------------------------------------------------------------------------------------------------------
inline HeaderMenu::HeaderMenu(const CRect& size, IControlListener* listener, int32_t tag, CBitmap* background, CBitmap* background_click)
  : COptionMenu(size, listener, tag, background, background_click)
{

}

} // namespace INVOXRecords
#endif // IR_GUIHEADERMENU_H