#include "Node.hpp"
#include <cmath>
#include "ngl/NGLStream.h"

Node::Node(Grid *_grid, std::vector<Node> *nodes, int _tile_id, int _target_id, int _parent_id):
  m_grid(_grid),
  m_id(nodes->size()),
  m_tile_id(_tile_id),
  m_target_id(_target_id),
  m_pos(m_grid->idToCoord(m_tile_id)),
  m_parent_id(_parent_id),
  m_open(true),
  m_g_cost(calcGCost(nodes, _parent_id)),
  m_h_cost(manhattanDist(_target_id))
{
  if(m_grid->read(m_tile_id) != Tile::EMPTY)
  {
    close();
  }
}

void Node::close()
{
  m_open = false;
}

float Node::calcGCost(std::vector<Node> *_nodes, int _parent_id)
{
  if(_parent_id == -1)
  {
    return 0;
  }
  else
  {
    return (*_nodes)[_parent_id].getGCost() + 1;
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

int Node::getID()
{
  return m_id;
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

void Node::setParent(std::vector<Node> *_nodes, int _parent_id)
{
  m_parent_id = _parent_id;
  m_g_cost = calcGCost(_nodes, m_parent_id);
}

int Node::getParentID()
{
  return m_parent_id;
}

std::array<int, 4> Node::getNeighbours(std::vector<Node> *_nodes)
{
  std::array<int, 4> neighbour_ids = {-1, -1, -1, -1};
  ngl::Vec2 pos = m_grid->idToCoord(m_tile_id);
  for(Node &node : *_nodes)
  {
    int id_dist = node.getTileID() - m_tile_id;
    if(id_dist == -m_grid->getW() && pos[0] > 0)
    {
      neighbour_ids[Neighbour::UP] = node.getTileID();
    }
    else if(id_dist == m_grid->getW() && pos[0] < m_grid->getW() - 1)
    {
      neighbour_ids[Neighbour::DOWN] = node.getTileID();
    }
    else if(id_dist == -1 && pos[1] > 0)
    {
      neighbour_ids[Neighbour::LEFT] = node.getTileID();
    }
    else if(id_dist == 1 && pos[1] < m_grid->getH() - 1)
    {
      neighbour_ids[Neighbour::RIGHT] = node.getTileID();
    }
  }
  return neighbour_ids;
}
