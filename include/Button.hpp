#ifndef __BUTTON_HPP__
#define __BUTTON_HPP__

#include "ngl/Vec2.h"

struct Button
{
  static int m_id_counter;
  const int m_id;
  ngl::Vec2 m_pos;
  ngl::Vec2 m_size;

  Button(ngl::Vec2 _pos, ngl::Vec2 _size);
  bool isInside(ngl::Vec2 _pos);
  void activate();
  static void resetIdCounter();
};

#endif//__BUTTON_HPP__
