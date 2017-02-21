#include "Node.hpp"
#include <cmath>
#include "ngl/NGLStream.h"

Node::Node(Grid *_grid, std::vector<Node> *_nodes, ngl::Vec2 _pos, ngl::Vec2 _target_pos, int _parent_id):
  m_grid(_grid),
  m_node_vector(_nodes),
  m_id(_nodes->size()),
  m_pos(_pos),
  m_parent_id(_parent_id),
  m_open(true),
  m_g_cost(calcNewGCost(_parent_id)),
  m_h_cost(manhattanDist(_target_pos))
{

	if(!m_grid->get(m_pos).isTraversable() && m_pos != _target_pos && _parent_id != -1)
  {
    close();
  }
}

void Node::close()
{
  m_open = false;
}

float Node::calcNewGCost(int _parent_id)
{
  if(_parent_id == -1)
  {
    return 0;
  }
  else
  {
    return (*m_node_vector)[_parent_id].getGCost() + 1;
  }
}

float Node::manhattanDist(ngl::Vec2 _target_pos)
{
  return fabs(_target_pos[0] - m_pos[0]) + fabs(_target_pos[1] - m_pos[1]);
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

ngl::Vec2 Node::getPos()
{
  return m_pos;
}

float Node::getGCost()
{
  return m_g_cost;
}

void Node::setParent(int _parent_id)
{
  m_parent_id = _parent_id;
  m_g_cost = calcNewGCost(m_parent_id);
}

int Node::getParentID()
{
  return m_parent_id;
}
std::array<int, 4> Node::getNeighbours()
{
  // find neighbours in parent vector
  std::array<int, 4> neighbour_ids = {-1, -1, -1, -1};
  for(Node &node : *m_node_vector)
  {
    if(&node != this)
    {
      ngl::Vec2 vec_to_node = node.getPos() - m_pos;
      float dist_sqaured = vec_to_node.lengthSquared();
      if(dist_sqaured <= 1)
      {
        if(vec_to_node.m_y == 1)
        {
          neighbour_ids[Neighbour::UP] = node.getID();
        }
        else if(vec_to_node.m_y == -1)
        {
          neighbour_ids[Neighbour::DOWN] = node.getID();
        }
        else if(vec_to_node.m_x == -1)
        {
          neighbour_ids[Neighbour::LEFT] = node.getID();
        }
        else if(vec_to_node.m_x == 1)
        {
          neighbour_ids[Neighbour::RIGHT] = node.getID();
        }
      }
    }
  }
  return neighbour_ids;
}
