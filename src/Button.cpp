#include "Button.hpp"
#include <iostream>

int Button::m_id_counter(0);

Button::Button(XAlignment _x_align, YAlignment _y_align, ngl::Vec2 _window_res, ngl::Vec2 _offset, ngl::Vec2 _size) :
  m_id(m_id_counter++),
  m_offset(_offset),
  m_pos(0,0),
  m_size(_size),
  m_x_align(_x_align),
  m_y_align(_y_align)
{
  updatePos(_window_res);
}

void Button::updatePos(ngl::Vec2 window_res)
{
  int x_pos = 0;
  int y_pos = 0;
  switch (m_x_align) {
  case XAlignment::LEFT:
    x_pos = m_offset.m_x;
    break;
  case XAlignment::CENTER:
    x_pos = (window_res.m_x - m_size.m_x) / 2 + m_offset.m_x;
    break;
  case XAlignment::RIGHT:
    x_pos = window_res.m_x + m_offset.m_x;
    break;
  default:
    break;
  }

  switch (m_y_align) {
  case YAlignment::TOP:
    y_pos = m_offset.m_y;
    break;
  case YAlignment::CENTER:
    y_pos = (window_res.m_y - m_size.m_y) / 2 + m_offset.m_y;
    break;
  case YAlignment::BOTTOM:
    y_pos = window_res.m_y + m_offset.m_y;
    break;
  default:
    break;
  }

  m_pos = ngl::Vec2(x_pos, y_pos);
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

ngl::Vec2 Button::getPos()
{
  return m_pos;
}

// static function
void Button::resetIdCounter()
{
  m_id_counter = 0;
}
