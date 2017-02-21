#ifndef __BUTTON_HPP__
#define __BUTTON_HPP__

#include "ngl/Vec2.h"

enum class XAlignment{
  LEFT,
  CENTER,
  RIGHT
};

enum class YAlignment{
  TOP,
  CENTER,
  BOTTOM
};

class Button
{
public:
  Button(XAlignment _x_align, YAlignment _y_align, ngl::Vec2 _screen_res, ngl::Vec2 _offset, ngl::Vec2 _size);
  void updatePos(ngl::Vec2 screen_res);
  bool isInside(ngl::Vec2 _pos);
  void activate();
  int getID();
  static void resetIdCounter();

private:
  static int m_id_counter;
  const int m_id;
  ngl::Vec2 m_offset;
  ngl::Vec2 m_pos;
  ngl::Vec2 m_size;
  XAlignment m_x_align;
  YAlignment m_y_align;
};

#endif//__BUTTON_HPP__
