#include "Button.hpp"
#include <iostream>

int Button::m_id_counter(0);

Button::Button(XAlignment _x_align, YAlignment _y_align, ngl::Vec2 _screen_res, ngl::Vec2 _offset, ngl::Vec2 _size) :
  m_id(m_id_counter++),
  m_offset(_offset),
  m_pos(0,0),
  m_size(_size),
  m_x_align(_x_align),
  m_y_align(_y_align)
{
  updatePos(_screen_res);
}

void Button::updatePos(ngl::Vec2 screen_res)
{
  switch (m_x_align) {
  case XAlignment::LEFT:

    break;
  case XAlignment::CENTER:

    break;
  case XAlignment::RIGHT:

    break;
  default:
    break;
  }

  switch (m_y_align) {
  case YAlignment::TOP:

    break;
  case YAlignment::CENTER:

    break;
  case YAlignment::BOTTOM:

    break;
  default:
    break;
  }
}

bool Button::isInside(ngl::Vec2 _pos)
{
  bool is_inside = false;
  if(_pos.m_x >= m_pos.m_x &&
     _pos.m_x < m_pos.m_x + m_size.m_x &&
     _pos.m_y >= m_pos.m_y &&
     _pos.m_y < m_pos.m_y + m_size.m_y)
  {
    is_inside = true;
  }
  return is_inside;
}

void Button::activate()
{
  std::cout << "button pressed:" << m_id << std::endl;
}

int Button::getID()
{
  return m_id;
}

// static function
void Button::resetIdCounter()
{
  m_id_counter = 0;
}
