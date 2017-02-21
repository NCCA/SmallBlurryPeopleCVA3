#include "Gui.hpp"

Gui::Gui()
{

}

void Gui::init(ngl::Vec2 _res)
{
  init();
  setResolution(_res);
}

void Gui::init()
{
  wipeButtons();
  createTestButtons();
}

void Gui::setResolution(ngl::Vec2 _res)
{
  m_win_w = _res.m_x;
  m_win_h = _res.m_y;
}

void Gui::click()
{
  if(m_selected_button_id >= 0 && (size_t)m_selected_button_id < m_buttons.size())
  {
    m_buttons[m_selected_button_id].activate();
  }
}

void Gui::mousePos(ngl::Vec2 _pos)
{
  bool button_selected = false;
  for(Button &button : m_buttons)
  {
    if(button.isInside(_pos))
    {
      m_selected_button_id = button.getID();
      button_selected = true;
    }
  }
  if(!button_selected)
  {
    m_selected_button_id = -1;
  }
}

void Gui::wipeButtons()
{
  m_buttons.clear();
  Button::resetIdCounter();
}

void Gui::createTestButtons()
{
  addButton(10, 10, 100, 100);
}

void Gui::addButton(double _x, double _y, double _w, double _h)
{
  m_buttons.push_back(Button(XAlignment::LEFT, YAlignment::TOP, ngl::Vec2(m_win_w, m_win_h), ngl::Vec2(_x, _y), ngl::Vec2(_w, _h)));
}
