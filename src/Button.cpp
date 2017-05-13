#include "Button.hpp"
#include <iostream>

int Button::m_id_counter(0);

Button::Button(Action _action, XAlignment _x_align, YAlignment _y_align, ngl::Vec2 _window_res, ngl::Vec2 _offset, ngl::Vec2 _size, const std::string &_text) :
  m_id(m_id_counter++),
  m_offset(_offset),
  m_pos(0,0),
  m_size(_size),
  m_x_align(_x_align),
  m_y_align(_y_align),
  m_action(_action),
  m_text(_text)
{
  updatePos(_window_res);
}

void Button::updatePos(ngl::Vec2 _window_res)
{
  int x_pos = 0;
  int y_pos = 0;
  switch (m_x_align) {
  case XAlignment::LEFT:
    x_pos = m_offset.m_x;
    break;
  case XAlignment::CENTER:
    x_pos = (_window_res.m_x - m_size.m_x) / 2 + m_offset.m_x;
    break;
  case XAlignment::RIGHT:
    x_pos = _window_res.m_x - (m_offset.m_x + m_size.m_x);
    break;
  default:
    break;
  }

  switch (m_y_align) {
  case YAlignment::TOP:
    y_pos = m_offset.m_y;
    break;
  case YAlignment::CENTER:
    y_pos = (_window_res.m_y - m_size.m_y) / 2 + m_offset.m_y;
    break;
  case YAlignment::BOTTOM:
    y_pos = _window_res.m_y - (m_offset.m_y + m_size.m_y);
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

Action Button::getAction() const
{
  return m_action;
}

int Button::getID()
{
  return m_id;
}

ngl::Vec2 Button::getPos()
{
  return m_pos;
}

ngl::Vec2 Button::getSize()
{
  return m_size;
}

const std::string &Button::getText()
{
  return m_text;
}

void Button::setText(const std::string &_text)
{
  m_text = _text;
}

bool Button::isPassive(bool character_selected)
{
  switch(m_action)
  {
  case Action::PASSIVE:
  case Action::PASSIVE_CHARACTER:
  case Action::CHAR_STATE:
  case Action::POPULATION:
  case Action::INV_WOOD:
  case Action::INV_BERRIES:
  case Action::INV_FISH:
    return true;
    break;
  case Action::BUILDHOUSE:
  case Action::BUILDSTORE:
  case Action::FORAGE:
  case Action::CENTRECAMERA:
    if(!character_selected) return true;
    else return false;
    break;
  default:
    return false;
    break;
  }
}

// static function
void Button::resetIdCounter()
{
  m_id_counter = 0;
}

void Button::move(ngl::Vec2 _move_vec)
{
  m_offset += _move_vec;
}

// Notification Button:

NotificationButton::NotificationButton(Action _action, XAlignment _x_align, YAlignment _y_align, ngl::Vec2 _window_res, ngl::Vec2 _offset, ngl::Vec2 _size, const std::string &_text, ngl::Vec2 _map_pos) :
  Button(_action, _x_align, _y_align, _window_res, _offset, _size, _text),
  m_map_pos(_map_pos),
  m_age(0)
{

}

void NotificationButton::incrementAge()
{
  m_age++;
}

int NotificationButton::getAge()
{
  return m_age;
}

ngl::Vec2 NotificationButton::getMapPos()
{
  return m_map_pos;
}

