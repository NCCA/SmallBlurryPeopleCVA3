#ifndef __GRID_HPP__
#define __GRID_HPP__

#include <vector>
#include <iostream>
#include <ngl/Vec2.h>
#include <ngl/Vec3.h>
#include "GridTile.hpp"

/// @file Grid.hpp
/// @brief The Grid class holds information about what is contained in each cell of the map.
/// The Grid class is a wrapper around a std::vector of Tile enums. The Tile enums illustrate
/// what is contained within each cell of the map, which can be used by other classes for
/// rendering and path finding.

/// @class Grid
/// @brief A wrapper around a std::vector used to store map information
class Grid
{
public:

  /// @brief default ctor that sets the grid to a default 50 by 50 set of empty tiles and runs the initialiser
  Grid();

  /// @brief updateScript loads the specified script and runs it to create a new map
  /// @param _script_path is the file path to the python script

  void updateScript(std::string _script_path, int _new_w = 30, int _new_h = 30, int _new_seed = 4);

  void printTrees();
  void printTypes();

  /// @brief getTriangles
  /// @return a vector of ngl vector 3's where each 3 specify a triangle to be drawn
  std::vector<ngl::Vec3> getTriangles();

  TileType getTileType(int _x, int _y);
  TileType getTileType(int _id);
  int getTileHeight(int _x, int _y);
  int getTileHeight(int _id);
  float getInterpolatedHeight(float _x, float _y);
  int getTileId(int _x, int _y);
  bool isTileTraversable(int _x, int _y);
  bool isTileTraversable(int _id);
  std::vector<ngl::Vec2> getTreePositions(int _x, int _y);
  int cutTileTrees(int _id, int _goal_amount);

  void setTileType(int _id, TileType _type);
  void setTileType(int _x, int _y, TileType _type);
  void setBuildState(int _id, float _value, TileType _type);
  void setBuildState(int _x, int _y, float _value, TileType _type);
  float getBuildState(int _id);
  float getBuildState(int _x, int _y);
  void addOccupant(int _id);
  void addOccupant(int _x, int _y);
  void removeOccupant(int _id);
  void removeOccupant(int _x, int _y);
  int getOccupants(int _id);
  int getOccupants(int _x, int _y);
  int getNumTrees(int _x, int _y);

  /// @brief converts a tile id to a coordinate, the tile id is the
  /// one dimensional coordinate of the tile
  /// @param [in] the tile id to convert
  /// @return a ngl::vec2 that is the 2d coordinate of the tile
  ngl::Vec2 idToCoord(int _tileId);

  /// @brief converts a coordinate to a tile id
  /// @param [in] the tile id to convert
  /// @return a ngl::vec2 that is the 2d coordinate of the tile
  int coordToId(ngl::Vec2 _coord);

  /// @brief returns grid width
  /// @return int width of grid
  int getW();

  /// @brief returns grid height
  /// @return int height of grid
  int getH();

  int getGlobalMountainHeight();
  int getGlobalWaterLevel();

  bool hasChanges();
  void resetHasChanges();



private:
  /// @brief runs the given python script to generate a map
  /// @param [in] the script that will set values in m_map
  void runCurrentScript(int _w, int _h, int _seed);

  /// @brief loads the script at the given path, returning the script in a single string
  /// @param [in] path to look for script
  /// @return a string containing the python script to be run
  void loadScript(std::string _script_path);

  /// @brief width of the grid
  int m_w;

  /// @brief height of the grid
  int m_h;

  /// @brief container for map information
  std::vector<GridTile> m_tiles;

  std::string m_script;

  int m_mountain_height = 0;
  int m_water_level = 0;

  bool m_has_changes = false;

};

#endif//__GRID_HPP__
