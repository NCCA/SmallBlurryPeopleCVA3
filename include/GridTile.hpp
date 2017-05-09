#ifndef __GRIDTILE_HPP__
#define __GRIDTILE_HPP__

#include "ngl/Vec2.h"
#include <vector>

/// @file GridTile.hpp
/// @brief The GridTile class is a storage space for the data contained in each tile of the map

enum class TileType{
  NONE,
  TREES,
  STUMPS,
  WATER,
  MOUNTAINS,
  HOUSE,
  STOREHOUSE,
  FOUNDATION_A,
  FOUNDATION_B,
  FOUNDATION_C,
  FOUNDATION_D
};

/// @class GridTile
/// @brief stores all of the data associated with each tile in the map
class GridTile
{
public:
  /// @brief tor that takes a tile id that represents its position on the map
  /// @param [in] the id of the tile
  GridTile(int _id);

  /// @brief returns a bool to indicate if the tile can be walked across, currently trees and
  /// buildings are not traversable
  bool isTraversable();

  TileType getType();

  void setType(TileType _t);

  int getHeight();

  void setHeight(int _height);

  /// @brief returns the number of trees
  int getNumTrees();

  /// @brief sets the number of trees, maybe a decrement tree would be useful?
  void setNumTrees(int _num_trees);

  int cutTrees(int _goal_amount);

  void setBuildState(float _value, TileType _type);

  float getBuildState() {return m_build_state;}

  std::vector<ngl::Vec2> getTreePositions();


private:
  TileType m_type = TileType::NONE;
  int m_height;
  int m_trees = 0;
  float m_build_state = 0.0;
  std::vector<ngl::Vec2> m_tree_positions;
};


#endif//__GRIDTILE_HPP__
