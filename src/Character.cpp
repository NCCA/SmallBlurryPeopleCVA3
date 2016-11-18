#include "Character.hpp"

#include "ngl/Random.h"

Character::Character(Grid *_grid):
  m_grid(_grid)
{
  ngl::Random *rand = ngl::Random::instance();
  rand->setSeed(0);

  int target_x = (int)rand->randomPositiveNumber(10);
  int target_y = (int)rand->randomPositiveNumber(10);
  ngl::Vec2 target_pos = ngl::Vec2(target_x, target_y);

  m_target_id = m_grid->coordToId(target_pos);
  target_pos = m_grid->idToCoord(m_target_id);


  update();
}

void Character::update()
{

}

void Character::draw()
{

}
