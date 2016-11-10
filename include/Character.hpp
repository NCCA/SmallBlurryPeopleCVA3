#ifndef __CHARACTER_HPP__
#define __CHARACTER_HPP__

#include "Grid.hpp"
#include "ngl/Vec2.h"

class Character
{
public:
  Character(Grid *_grid);
  ~Character() = default;

  void update();
  void draw();


private:
  Grid *m_grid;
  ngl::Vec2 m_pos;
  int m_pos_id;
  int m_target_id;
};

#endif//__CHARACTER_HPP__
