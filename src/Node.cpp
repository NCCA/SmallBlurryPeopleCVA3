#include "Node.hpp"
#include <cmath>

Node::Node(Grid *_grid, int _tile_id, int _target_id, Node *_parent):
  m_grid(_grid),
  m_tile_id(_tile_id),
  m_pos(m_grid->idToCoord(m_tile_id)),
  m_parent(_parent),
  m_open(true),
  m_g_cost(calcGCost()),
  m_h_cost(manhattanDist(_target_id))
{

}

void Node::close()
{
  m_open = false;
}

float Node::calcGCost()
{
  if(m_parent == nullptr)
  {
    return 0;
  }
  else
  {
    ngl::Vec2 parent_pos = m_parent->getPos();

    if(parent_pos[0] == m_pos[0] || parent_pos[1] == m_pos[1])
    {
      return 1;
    }
    else
    {
      return 1.4142;
    }
  }
}

float Node::manhattanDist(int _target_id)
{
  ngl::Vec2 target_pos = m_grid->idToCoord(_target_id);
  return fabs(target_pos[0] - m_pos[0]) + fabs(target_pos[1] - m_pos[1]);
}

float Node::getFCost()
{
  return m_g_cost + m_h_cost;
}

bool Node::isOpen()
{
  return m_open;
}

int Node::getTileID()
{
  return m_tile_id;
}

ngl::Vec2 Node::getPos()
{
  return m_pos;
}

float Node::getGCost()
{
  return m_g_cost;
}
