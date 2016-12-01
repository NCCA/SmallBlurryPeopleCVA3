#include "Character.hpp"

#include "Node.hpp"

#include "ngl/Random.h"
#include "ngl/NGLStream.h"

#include <iostream>

Character::Character(Grid *_grid):
  m_grid(_grid)
{
  ngl::Random *rand = ngl::Random::instance();
  rand->setSeed(0);

  m_pos = rand->getRandomVec2();
  m_pos *= 50;
  m_pos[0] = fabs(m_pos[0]);
  m_pos[1] = fabs(m_pos[1]);

  m_target_id = m_grid->coordToId(m_pos);

  setTarget(100);

  std::cout << "character created" << std::endl;
  std::cout << "character target: " << m_target_id << std::endl;
  update();
}

void Character::update()
{
  std::cout << "character updated" << std::endl;
  draw();
}

void Character::draw()
{
  std::cout << "character position: " << m_pos << std::endl;
  std::cout << "character drawn" << std::endl;
}

void Character::findPath()
{

}

void Character::setTarget(ngl::Vec2 _target_pos)
{
  // convert to tile id
  setTarget(m_grid->coordToId(_target_pos));
}

void Character::setTarget(int _tile_id)
{
  if(_tile_id != m_target_id)
  {
    m_target_id = _tile_id;
    findPath();
  }
}
