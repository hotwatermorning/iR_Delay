//------------------------------------------------------------------------
// Copyright(c) 2021 iNVOX Records.
//------------------------------------------------------------------------

#include "iR_GUIBase.h"

#include "vstgui/lib/controls/coptionmenu.h"

namespace INVOXRecords {
//------------------------------------------------------------------------
// iRGUIBase
//------------------------------------------------------------------------
iRGUIBase::iRGUIBase(void* controller, int FrameWidth, int FrameHeight) : VSTGUIEditor(controller)
{
  this->FrameWidth = FrameWidth;
  this->FrameHeight = FrameHeight;
  ViewRect viewRect(0, 0, FrameWidth, FrameHeight);
  setRect(viewRect);
}

//------------------------------------------------------------------------
bool PLUGIN_API iRGUIBase::open(void* parent, const PlatformType& platformType)
{
  if (frame) return false;

  CRect size(0, 0, FrameWidth, FrameHeight);

  frame = new CFrame(size, this);
  if (!frame) return false;

  frame->setBackgroundColor(CColor(30, 30, 30));
  frame->open(parent);

  return true;
}

//------------------------------------------------------------------------
void PLUGIN_API iRGUIBase::close()
{
  if (frame) {
    frame->forget();
    frame = 0;
  }
}

void iRGUIBase::updateUI()
{
  for (auto&& control : control_container) {
    if (control) {
      Vst::ParamID index = control->getTag();
      Vst::ParamValue value = controller->getParamNormalized(index);
      control->setValueNormalized(value);
      control->setDirty();
      updateValueProcess(index, value);
    }
  }
}

//------------------------------------------------------------------------
// On-Off Button
COnOffButton* iRGUIBase::createOnOffButton(Vst::ParamID tag, int x, int y, const CResourceDescription& filename)
{
  CBitmap* bmp = new CBitmap(filename);

  CRect size(0, 0, bmp->getWidth(), bmp->getHeight() / 2);
  size.offset(x, y);

  COnOffButton* control = new COnOffButton(size, this, tag, bmp);
  bmp->forget();

  Vst::ParamValue value = controller->getParamNormalized(tag);
  control->setValueNormalized(value);

  frame->addView(control);


  return control;
}

//------------------------------------------------------------------------
// Vertical Slider
CVerticalSlider* iRGUIBase::createVerticalSlider(
  Vst::ParamID tag, int x, int y, const CResourceDescription& background, const CResourceDescription& handle)
{
  CBitmap* backbmp = new CBitmap(background);
  CBitmap* handlebmp = new CBitmap(handle);

  CRect  size;
  size(0, 0, backbmp->getWidth(), backbmp->getHeight());
  size.offset(x, y);

  CVerticalSlider* control = new CVerticalSlider(size, this, tag, y,
    y + backbmp->getHeight() - (handlebmp->getHeight()), handlebmp, backbmp);

  Vst::ParamValue value = controller->getParamNormalized(tag);
  control->setValueNormalized(value);

  frame->addView(control);

  backbmp->forget();
  handlebmp->forget();

  return control;
}

//------------------------------------------------------------------------
// Kick Button
CKickButton* iRGUIBase::createKickButton(Vst::ParamID tag, int x, int y, const CResourceDescription& filename)
{
  CBitmap* bmp = new CBitmap(filename);

  CRect size(0, 0, bmp->getWidth(), bmp->getHeight() / 2);
  size.offset(x, y);

  CKickButton* control = new CKickButton(size, this, tag, bmp);
  bmp->forget();

  Vst::ParamValue value = controller->getParamNormalized(tag);
  control->setValueNormalized(value);

  frame->addView(control);

  return control;
}

//------------------------------------------------------------------------
// Text Button
CTextButton* iRGUIBase::createTextButton(Vst::ParamID tag, int x, int y, int width, int height, UTF8StringPtr title)
{
  CRect size(0, 0, width, height);
  size.offset(x, y);

  CTextButton* control = new CTextButton(size, this, tag, title);

  Vst::ParamValue value = controller->getParamNormalized(tag);
  control->setValueNormalized(value);

  frame->addView(control);

  return control;
}

//------------------------------------------------------------------------
// Check Box
CCheckBox* iRGUIBase::createCheckBox(Vst::ParamID tag, int x, int y, UTF8StringPtr title, const CResourceDescription& filename = nullptr)
{
  CRect size;
  CBitmap* bmp = nullptr;
  if (&filename != nullptr) {
    bmp = new CBitmap(filename);
    size = size(0, 0, bmp->getWidth(), bmp->getHeight() / 2);
  }
  else {
    size = size(0, 0, 120, 12);
  }

  size.offset(x, y);

  CCheckBox* control;
  if (bmp) {
    control = new CCheckBox(size, this, tag, title, bmp);
    bmp->forget();
  }
  else {
    control = new CCheckBox(size, this, tag, title);
  }

  Vst::ParamValue value = controller->getParamNormalized(tag);
  control->setValueNormalized(value);

  frame->addView(control);

  return control;
}

ControlKnob* iRGUIBase::createKnob(Vst::ParamID tag, int x, int y, UTF8StringPtr title, KnobStartPoint start_point)
{
  CBitmap* bmp;
  if (start_point == KnobStartPoint::kCenter) {
    bmp = new CBitmap("KNOB_START_CENTER");
  } else if (start_point == KnobStartPoint::kLeft) {
    bmp = new CBitmap("KNOB_START_LEFT");
  } else {
    bmp = new CBitmap("KNOB_START_RIGHT");
  }
  CRect size(0, 0, bmp->getWidth(), bmp->getHeight() / 65);
  size.offset(x + 10, y + 20);

  ControlKnob* control = new ControlKnob(size, this, tag, bmp, title);
  bmp->forget();

  Vst::ParamValue value = controller->getParamNormalized(tag);
  control->setValueNormalized(value);

  frame->addView(control);
  frame->addView(control->getTitleLabelPtr());
  frame->addView(control->getValueLabelPtr());

  control_container.push_back(control);
  control_container.push_back(control->getValueLabelPtr());

  knob_container.push_back(control);

  return control;
}

void iRGUIBase::setAlwaysShowAllKnobValue(bool state)
{
  for (auto&& knob : knob_container) {
    knob->setAlwaysShowValueLabel(state);
  }
}

ControlSwitch* iRGUIBase::createSwitch(Vst::ParamID tag, int x, int y, UTF8StringPtr title)
{
  CBitmap* bmp = new CBitmap("SWITCH");

  CRect size(0, 0, bmp->getWidth(), bmp->getHeight() / 2);
  size.offset(x + 10, y + 20);

  ControlSwitch* control = new ControlSwitch(size, this, tag, bmp, title);
  bmp->forget();

  Vst::ParamValue value = controller->getParamNormalized(tag);
  control->setValueNormalized(value);

  frame->addView(control);
  frame->addView(control->getTitleLabelPtr());

  control_container.push_back(control);

  return control;
}

ControlMenu* iRGUIBase::createMenu(Vst::ParamID tag, int x, int y, UTF8StringPtr title, MenuSize menu_size)
{
  CBitmap* bmp_bg;
  CBitmap* bmp_bg_click;
  if (menu_size == MenuSize::kShort) {
    bmp_bg = new CBitmap("MENU_SHORT");
    bmp_bg_click = new CBitmap("MENU_SHORT_CLICK");
  } else if (menu_size == MenuSize::kMidium) {
    bmp_bg = new CBitmap("MENU_MIDIUM");
    bmp_bg_click = new CBitmap("MENU_MIDIUM_CLICK");
  } else {
    bmp_bg = new CBitmap("MENU_LONG");
    bmp_bg_click = new CBitmap("MENU_LONG_CLICK");
  }
  CRect size(0, 0, bmp_bg->getWidth(), bmp_bg->getHeight());
  size.offset(x + 10, y + 33);

  ControlMenu* control = new ControlMenu(size, this, tag, bmp_bg, bmp_bg_click, title);
  bmp_bg->forget();
  bmp_bg_click->forget();

  Vst::ParamValue value = controller->getParamNormalized(tag);
  control->setValueNormalized(value);

  control->setFont(kNormalFontVeryBig);

  frame->addView(control);
  frame->addView(control->getTitleLabelPtr());

  control_container.push_back(control);

  return control;
}

CategoryField* iRGUIBase::createField(CRect size, UTF8StringPtr title, CColor color)
{
  CategoryField* view = new CategoryField(size, title, color);
  frame->addView(view);
  frame->addView(view->getTitleLabelPtr());

  return view;
}

Header* iRGUIBase::createHeader(UTF8StringPtr title, Vst::ParamID header_menu_tag)
{
  CRect size(-1, 0, FrameWidth + 1, 40);
  Header* view = new Header(size, title);
  ControlMenu* header_menu = createHeaderMenu(header_menu_tag);

  view->getTitleLabelPtr()->setFont(kPluginTitleFont);
  view->getCopyrightLabelPtr()->setFont(kNormalFontSmaller);
  view->setHeaderMenu(header_menu);

  frame->addView(view);
  frame->addView(view->getTitleLabelPtr());
  frame->addView(view->getCopyrightLabelPtr());
  frame->addView(view->getHeaderMenu());

  return view;
}

ControlMenu* iRGUIBase::createHeaderMenu(Vst::ParamID tag)
{
  CBitmap* bmp = new CBitmap("HEADER_MENU");
  CBitmap* bmp_click = new CBitmap("HEADER_MENU_CLICK");
  CRect size(0, 0, bmp->getWidth(), 40);
  size.offset(FrameWidth - 30, 7.5f);
  ControlMenu* menu = new ControlMenu(size, this, tag, bmp, bmp_click, nullptr);
  bmp->forget();
  bmp_click->forget();

  menu->setFontColor(kTransparentCColor);
  menu->setStyle(ControlMenu::kMultipleCheckStyle);
  menu->addEntry(u8"Always show knob value");
  menu->addEntry(u8"Parameter Tips");
  menu->checkEntry(1, true);

  return menu;
}

//------------------------------------------------------------------------
// Text Label
CTextLabel* iRGUIBase::createTextLabel(int x, int y, int width, int height, UTF8StringPtr text)
{
  CRect  size;
  size(0, 0, width, height);
  size.offset(x, y);

  CTextLabel* control = new CTextLabel(size, text);

  control->setFont(kNormalFontSmaller);
  control->setFontColor(kBlackCColor);
  control->setBackColor(kTransparentCColor);
  control->setFrameColor(kTransparentCColor);
  control->setMouseableArea(CRect(0, 0, 0, 0));

  frame->addView(control);

  return control;
}

//------------------------------------------------------------------------
void iRGUIBase::setTextLabel(Vst::ParamID index, Vst::ParamValue value, CControl* label,
  UTF8StringPtr prefix = nullptr, UTF8StringPtr suffix = nullptr)
{
  Vst::String128 tmp;
  controller->getParamStringByValue(index, value, tmp);
  char text[128];
  wcstombs(text, tmp, 128);

  if (!prefix && !suffix) {
    ((CTextLabel*)label)->setText(text);
    label->setDirty();
    return;
  }

  char result_text[256];
  if (prefix) {
    memcpy(result_text, prefix, 64);
    memcpy(result_text + std::strlen(prefix), text, 128);
    if (suffix) 
      memcpy(result_text + std::strlen(prefix) + std::strlen(text), suffix, 64);
  } else {
    memcpy(result_text, text, 64);
    memcpy(result_text + std::strlen(text), suffix, 64);
  }
  ((CTextLabel*)label)->setText(result_text);
  label->setDirty();
}

//------------------------------------------------------------------------
// Text Edit
CTextEdit* iRGUIBase::createTextEdit(Vst::ParamID tag, int x, int y, int width, int height, UTF8StringPtr text)
{
  CRect  size;
  size(0, 0, width, height);
  size.offset(x, y);

  CTextEdit* control = new CTextEdit(size, this, tag, text);

  control->setFont(kNormalFontSmaller);
  control->setStyle(CTextEdit::kDoubleClickStyle);

  control->setFontColor(kBlackCColor);
  control->setBackColor(kTransparentCColor);
  control->setFrameColor(kTransparentCColor);

  frame->addView(control);

  return control;
}

//------------------------------------------------------------------------
float iRGUIBase::stringToValue(CTextEdit* edit, float min, float max)
{
  float value = strtof(((CTextEdit*)edit)->getText(), nullptr);
  if (value < min) value = min;
  if (value > max) value = max;
  return value;
}
//------------------------------------------------------------------------
} // namespace INVOXRecords
