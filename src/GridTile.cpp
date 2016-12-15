#include "GridTile.hpp"

GridTile::GridTile(int id):
  m_id(id),
  m_trees(0),
  m_height(0.0),
  m_building(BuildingType::NONE)
{
}

bool GridTile::isTraversable()
{
  if(m_trees == 0 && m_building == BuildingType::NONE)
  {
    return true;
  }
  return false;
}

bool GridTile::isBuilding()
{
  if(m_building == BuildingType::NONE)
  {
    return false;
  }
  return true;
}

bool GridTile::isTrees()
{
  if(m_trees == 0)
  {
    return false;
  }
  return true;
}


BuildingType GridTile::getBuildingType()
{
  return m_building;
}

int GridTile::getNumTrees()
{
  return m_trees;
}

float GridTile::getHeight()
{
  return m_height;
}

int GridTile::getID()
{
  return m_id;
}

void GridTile::setBuildingType(BuildingType _type)
{
  m_building = _type;
}

void GridTile::setNumTrees(int _num_trees)
{
  m_trees = _num_trees;
}

void GridTile::setHeight(float _height)
{
  m_height = _height;
}




