#include "GridTile.hpp"
#include <iostream>

GridTile::GridTile(int _id):
	m_id(_id),
	m_build_state(0.0)
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

void GridTile::setBuildState(float _value, TileType _type)
{
	m_build_state += _value;
	std::cout<<"Building Done: "<<(m_build_state * 100)<<"%"<<std::endl;

	if (_type == TileType::HOUSE)
	{
		if(m_build_state >= 0.0 && m_build_state < 0.5)
			m_type = TileType::FOUNDATION_A;
		else if(m_build_state >= 0.5 && m_build_state < 1.0)
			m_type = TileType::FOUNDATION_B;
		else if(m_build_state >= 1.0)
			m_type = TileType::HOUSE;
	}
	else if (_type == TileType::STOREHOUSE)
	{
		if(m_build_state >= 0.0 && m_build_state < 0.5)
			m_type = TileType::FOUNDATION_C;
		else if(m_build_state >= 0.5 && m_build_state < 1.0)
			m_type = TileType::FOUNDATION_D;
		else if(m_build_state >= 1.0)
			m_type = TileType::STOREHOUSE;
	}
}




