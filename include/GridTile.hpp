#ifndef __GRIDTILE_HPP__
#define __GRIDTILE_HPP__

/// @file GridTile.hpp
/// @brief The GridTile class is a storage space for the data contained in each tile of the map

/// @enum An enum class to make identifying the building type easier, may not be necessary later on
enum class BuildingType{
  NONE,
  HOUSE,
  STOREHOUSE
};

enum class TileType{
  NONE,
  TREES,
  WATER,
  MOUNTAINS,
  HOUSE,
  STOREHOUSE
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

  /// @brief returns the id of the tile
  int getID();

  /// @brief sets the number of trees, maybe a decrement tree would be useful?
  void setNumTrees(int _num_trees);

  int cutTrees(int _goal_amount);

private:
  int m_id;
  TileType m_type = TileType::NONE;
  int m_height;
  int m_trees = 0;
};


#endif//__GRIDTILE_HPP__
