#include "GridTile.hpp"
#include <iostream>

GridTile::GridTile(int _id):
  m_id(_id)
{
}

bool GridTile::isTraversable()
{
  if(m_type == TileType::NONE)
  {
    return true;
  }
  return false;
}

TileType GridTile::getType()
{
  return m_type;
}

void GridTile::setType(TileType _t)
{
  m_type = _t;
}

int GridTile::getHeight()
{
  return m_height;
}

void GridTile::setHeight(int _height)
{
  m_height = _height;
}

int GridTile::getNumTrees()
{
  return m_trees;
}

int GridTile::getID()
{
  return m_id;
}

void GridTile::setNumTrees(int _num_trees)
{
  m_trees = _num_trees;
}

int GridTile::cutTrees(int _goal_amount)
{
  int output = 0;
  if (m_trees <= _goal_amount)
  {
    output = m_trees;
    m_trees = 0;
    m_type = TileType::NONE;
  }
  else
  {
    m_trees -= _goal_amount;
    output = _goal_amount;
  }
  return output;
}




