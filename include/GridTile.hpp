#ifndef __GRIDTILE_HPP__
#define __GRIDTILE_HPP__

#include "ngl/Vec2.h"
#include <vector>

/// @file GridTile.hpp
/// @brief header file for the GridTile class

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
  ///
  /// @brief ctor that takes a tile id that represents its position on the map
  /// @param the id of the tile
  ///
  GridTile(int _id);

  ///
  /// \brief isTraversable returns a bool to indicate if the tile can be walked across, currently trees and
  /// buildings are not traversable
  /// \return bool returned
  ///
  bool isTraversable();

  ///
  /// \brief getType gets the type of the tile
  /// \return the TileType stored in the tile
  ///
  TileType getType();

  ///
  /// \brief setType sets the type of the tile
  /// \param _t is the type to be set
  ///
  void setType(TileType _t);

  ///
  /// \brief getHeight gets the height stored in the tile
  /// \return the height of the tile
  ///
  int getHeight();

  ///
  /// \brief setHeight sets the height of the tile
  /// \param _height is the height to be set
  ///
  void setHeight(int _height);

  ///
  /// \brief getNumTrees returns the number of trees on a tile
  /// \return the number of trees left on the tile
  ///
  int getNumTrees();

  ///
  /// \brief setNumTrees sets the number of trees stored in the tile
  /// \param _num_trees is the number of trees to be set
  ///
  void setNumTrees(int _num_trees);

  ///
  /// \brief cutTrees try to reduce the number of treesm returning the number of trees cut
  /// \param _goal_amount is the desired abount of trees to cut
  /// \return the actualy amount of trees cut (for instances where 3 are reuqested but ony one is there for example)
  ///
  int cutTrees(int _goal_amount);

  ///
  /// \brief setBuildState sets the build state for a building tile
  /// \param _value is the amount to increment the build state variable by
  /// \param _type is the type of building being built
  ///
  void setBuildState(float _value, TileType _type);

  ///
  /// \brief getBuildState gets the completion of a building in the range [0 ... 1]
  /// \return the tiles build state
  ///
  float getBuildState() {return m_build_state;}

  ///
  /// \brief getTreePositions gets the positions of the trees on a tile
  /// \return a vector of Vec2's that store the trees positions as offsets of the tiles position [-0.4 ... 0.4]
  ///
  std::vector<ngl::Vec2> getTreePositions();

private:
  ///
  /// \brief m_type is the type of the tile
  ///
  TileType m_type = TileType::NONE;

  ///
  /// \brief m_height is the height of the tile
  ///
  int m_height = 0;

  ///
  /// \brief m_trees is the number of trees on the tile
  ///
  int m_trees = 0;

  ///
  /// \brief m_build_state is the build state of the tile
  ///
  float m_build_state = 0.0;

  ///
  /// \brief m_tree_positions stores the positions of trees on the tile as offsets in the range [-0.4, 0.4] from the tiles position
  ///
  std::vector<ngl::Vec2> m_tree_positions;
};

#endif//__GRIDTILE_HPP__
