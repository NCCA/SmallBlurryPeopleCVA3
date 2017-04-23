#include "Baddie.hpp"
#include "NodeNetwork.hpp"
#include "Prefs.hpp"
#include "ngl/NGLStream.h"
#include "Utility.hpp"
#include <algorithm>

Baddie::Baddie(Grid *_grid) :
  m_grid(_grid),
  m_path()
{
  m_pos = ngl::Vec2(0.5,0.5);
  m_target_id = m_grid->coordToId(m_pos);
  m_health = 1.0f;
  Prefs* prefs = Prefs::instance();
  m_speed = prefs->getFloatPref("CHARACTER_SPEED");// * 0.25;

  setTarget(ngl::Vec2(m_grid->getW()/2, m_grid->getH()/2));
}

void Baddie::update(const std::vector<Character> &m_characters)
{
  if(move())
  {
    targetBest(m_characters);
  }

  //setTarget(target);

  //std::cout << "-Baddie-" << std::endl;
  //std::cout << "pos:" << m_pos << std::endl;
}

bool Baddie::move()
{
  // check whether the next point is necessary or if character has line of sight to the second point in list
  if(m_path.size() > 1 && NodeNetwork::raytrace(m_grid, m_pos, m_path.end()[-2]))
  {
    // if character can see point after next, remove the next point so character heads directly to the furtherst one they can see
    // done on a one by one basis so it does all happen in the same frame as pathfinding, not super slow that way but marginally better this way
    m_path.pop_back();
  }

  // move by distance up to speed
  float dist_moved = 0;
  while(m_path.size() > 0 && dist_moved < m_speed)
  {
    float dist = 1;
    ngl::Vec2 aim_vec = calcAimVec(&dist);
    if(dist < m_speed)
    {
      aim_vec *= dist;
      m_path.pop_back();
    }
    else
    {
      aim_vec *= m_speed;
    }
    dist_moved += aim_vec.length();
    m_pos += aim_vec;
  }
  updateRot();
  if(m_path.size() <= 0)
  {
    return true;
  }
  else return false;
}

ngl::Vec2 Baddie::calcAimVec(float *dist)
{
  ngl::Vec2 aim_vec(0,0);
  if(m_path.size() > 0)
  {
   aim_vec = m_path.back() - m_pos;
  }
  if(dist)
  {
    *dist = aim_vec.length();
  }
  if(aim_vec != ngl::Vec2(0,0))
  {
    aim_vec.normalize();
  }
  return aim_vec;
}

bool Baddie::setTarget(ngl::Vec2 _target_pos)
{
  if(m_target_id != m_grid->coordToId(_target_pos))
  {
    m_target_id = m_grid->coordToId(_target_pos);
    findPath(_target_pos);
  }
  if(m_path.size() <= 0)
  {
    return false;
  }
  else
  {
    return true;
  }
}

void Baddie::findPath(ngl::Vec2 _target)
{
  NodeNetwork network(m_grid, m_pos, _target);
  m_path = network.findPath();
}

ngl::Vec3 Baddie::getPos()
{
  //get grid height at baddie's position
  float height = m_grid->getInterpolatedHeight(m_pos[0], m_pos[1]);
  return ngl::Vec3(m_pos[0], height, m_pos[1]);
}

void Baddie::updateRot()
{
  if(m_path.size() > 0)
  {
    ngl::Vec2 dir = m_path.back() - m_pos;
    m_rot = Utility::degrees(atan2(dir.m_x, dir.m_y));
  }
}

bool Baddie::targetBest(const std::vector<Character> &_characters)
{
  std::vector<ngl::Vec2> positions;
  for(const Character &c : _characters)
  {
    positions.push_back(c.getPos2d());
  }
  return false;
}


