#include "Gui.hpp"

Gui::Gui()
{

}

void Gui::init()
{
  wipeButtons();
  createTestButtons();
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
      m_selected_button_id = button.m_id;
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
  addButton(0.1, 0.1, 0.2, 0.2);
  addButton(0.7, 0.1, 0.2, 0.2);
}

void Gui::addButton(double _x, double _y, double _w, double _h)
{
  m_buttons.push_back(Button(ngl::Vec2(_x, _y), ngl::Vec2(_w, _h)));
}
