#include "Button.hpp"
#include <iostream>

int Button::m_id_counter(0);

Button::Button(ngl::Vec2 _pos, ngl::Vec2 _size) :
  m_id(m_id_counter++),
  m_pos(_pos),
  m_size(_size)
{

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

void Button::resetIdCounter()
{
  m_id_counter = 0;
}
