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

  void updateScript(std::string _script_path);

  void printTrees();
  void printTypes();

  std::vector<ngl::Vec3> getTriangles();

  /// @brief returns the tile at the specified width and height coordinate.
  /// @param [in] _x is the x coordinate of the requested tile
  /// @param [in] _y is the y coordinate of the requested tile
  /// @return returns the GridTile at the given coordinate
  GridTile get(int _x, int _y);

  /// @brief returns the tile at the specified coordinate.
  /// @param [in] _coord is the coordinate as a ngl::Vec2
  /// @return returns the Tile at the given coordinate
  GridTile get(ngl::Vec2 _coord);

  /// @brief returns the tile at the specified width and height coordinate.
  /// @param [in] _id is the 1d coordinate of the tile
  /// @return returns the Tile at the given coordinate
  GridTile get(int _id);

  /// @brief sets the tile at a given coordinate
  /// @param [in] _x is the x coordinate of the tile to be set
  /// @param [in] _y is the y coordinate of the tile to be set
  /// @param [in] _t is the value to be set at the given coordiate
  void set(int _x, int _y, GridTile _t);

  /// @brief sets the tile at a given coordinate
  /// @param [in] _coord is the coordinate as a ngl::Vec2
  /// @param [in] _t is the value to be set at the given coordiate
  void set(ngl::Vec2 _coord, GridTile _t);

  /// @brief sets the value of a tile at a given coordinate
  /// @param [in] _id is the 1d coord of the tile
  /// @param [in] _t is the tile to be set at the given coordiate
  void set(int _id, GridTile _t);

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

private:

  /// @brief runs the given python script to generate a map
  /// @param [in] the script that will set values in m_map
  void runCurrentScript();

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
};

#endif//__GRID_HPP__
