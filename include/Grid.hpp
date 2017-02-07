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

  /// @brief default dtor
  ~Grid() = default;

  void updateScript(std::string _script_path);

  void genMap();

  /// @brief prints the map to the terminal
  void print();

  void print2();

  void printHeight();
  //void printType();
  void printTrees();

  std::vector<ngl::Vec3> getTriangles();

  /// @brief returns the value of a tile at the specified width and height coordinate.
  /// It does not perform any error checking that the requested tile exists and will
  /// return garbage if the index is out of range.
  /// @param [in] _x is the x coordinate of the requested tile
  /// @param [in] _y is the y coordinate of the requested tile
  /// @return returns the Tile at the given coordinate
  //Tile read(int _x, int _y);
  GridTile get(int _x, int _y);

  /// @brief returns the value of a tile at the specified coordinate.
  /// It does not perform any error checking that the requested tile exists and will
  /// return garbage if the index is out of range.
  /// @param [in] _coord is the coordinate as a ngl::Vec2
  /// @return returns the Tile at the given coordinate
  //Tile read(ngl::Vec2 _coord);
  GridTile get(ngl::Vec2 _coord);

  /// @brief returns the value of a tile at the specified width and height coordinate.
  /// It does not perform any error checking that the requested tile exists and will
  /// return garbage if the index is out of range.
  /// @param [in] _id is the 1d coordinate of the tile
  /// @return returns the Tile at the given coordinate
  //Tile read(int _id);
  GridTile get(int _id);

  /// @brief sets the value of a tile at a given coordinate
  /// It does not perform any error checking that the requested tile exists.
  /// @param [in] _x is the x coordinate of the tile to be set
  /// @param [in] _y is the y coordinate of the tile to be set
  /// @param [in] _t is the value to be set at the given coordiate
  //void write(int _x, int _y, Tile _t);
  void set(int _x, int _y, GridTile _t);

  /// @brief sets the value of a tile at a given coordinate
  /// It does not perform any error checking that the requested tile exists.
  /// @param [in] _coord is the coordinate as a ngl::Vec2
  /// @param [in] _t is the value to be set at the given coordiate
  //void write(ngl::Vec2 _coord, Tile _t);
  void set(ngl::Vec2 _coord, GridTile _t);

  /// @brief sets the value of a tile at a given coordinate
  /// It does not perform any error checking that the requested tile exists.
  /// @param [in] _id is the 1d coord of the tile
  /// @param [in] _t is the value to be set at the given coordiate
  //void write(int _id, Tile _t);
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

  /// @brief generates a new layout from the given script, if no
  /// script can be found, it will use the generateRandomMap()
  /// function
  /// @param [in] the file path to the script, relative to the programs directory
  void newLayout(std::string _script_path);

  /// @brief returns grid width
  /// @return int width of grid
  int getW();
  /// @brief returns grid height
  /// @return int height of grid
  int getH();

private:

  void resize(int _w, int _h);

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
