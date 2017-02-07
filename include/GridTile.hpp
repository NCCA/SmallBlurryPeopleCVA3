#ifndef __GRIDTILE_HPP__
#define __GRIDTILE_HPP__

/// @file GridTile.hpp
/// @brief The GridTile class is a storage space for the data contained in each tile of the map

/// @enum An enum class to make identifying the building type easier, may not be necessary later on
enum class BuildingType{
  NONE,
  HOUSE
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

  /// @brief returns a bool to indicate if the tile is a building
  bool isBuilding();

  ///@brief returns a bool that indicates if the tile contains trees
  bool isTrees();

  /// @brief returns the type of building
  BuildingType getBuildingType();

  /// @brief returns the number of trees
  int getNumTrees();

  /// @brief returns the height of the tile
  float getHeight();

  /// @brief returns the id of the tile
  int getID();

  /// @brief sets the building type
  void setBuildingType(BuildingType _type);

  /// @brief sets the number of trees, maybe a decrement tree would be useful?
  void setNumTrees(int _num_trees);

  /// @brief sets the height of the tile
  void setHeight(float _height);

private:
  int m_id;
  BuildingType m_building = BuildingType::NONE;
  int m_trees = 0;
  float m_height = 0;
};


#endif//__GRIDTILE_HPP__
