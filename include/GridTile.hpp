#ifndef __GRIDTILE_HPP__
#define __GRIDTILE_HPP__

enum class BuildingType{
  NONE,
  HOUSE
};


class GridTile
{
public:
  GridTile(int id);

  bool isTraversable();
  bool isBuilding();
  bool isTrees();

  BuildingType getBuildingType();
  int getNumTrees();
  float getHeight();
  int getID();

  void setBuildingType(BuildingType _type);
  void setNumTrees(int _num_trees);
  void setHeight(float _height);

private:
  BuildingType m_building;
  int m_trees;
  float m_height;
  int m_id;
};


#endif//__GRIDTILE_HPP__
