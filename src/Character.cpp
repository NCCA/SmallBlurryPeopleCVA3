#include "Character.hpp"

#include "ngl/Random.h"

Character::Character(Grid *_grid):
  m_grid(_grid)
{
  ngl::Random rand = ngl::Random::instance();
  rand->setSeed(0);

  int target_x = (int)rand->randomPositiveNumber(10);
  int target_y = (int)rand->randomPositiveNumber(10);

  // m_target_id = grid->getTileId(target_x, target_y);
  // m_grid->getTilePos(tile_id, x_pos, y_pos);
}

void Character::update()
{

}

void Character::draw()
{

}
