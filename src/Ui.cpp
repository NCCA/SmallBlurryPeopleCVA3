#include "Ui.hpp"

Ui::Ui()
{
  testButtons();
}

void Ui::click()
{
  m_buttons[m_selected_button_id].activate();
}

void Ui::mousePos(ngl::Vec2 _pos)
{
  for(Button &button : m_buttons)
  {
    if(button.isInside(_pos))
    {
      m_selected_button_id = button.m_id;
    }
  }
}

void Ui::wipeButtons()
{
  m_buttons.clear();
  Button::resetIdCounter();
}

void Ui::testButtons()
{
  addButton(0.1, 0.1, 0.2, 0.2);
  addButton(0.7, 0.1, 0.2, 0.2);
}

void Ui::addButton(double _x, double _y, double _w, double _h)
{
  m_buttons.push_back(Button(ngl::Vec2(_x, _y), ngl::Vec2(_w, _h)));
}
