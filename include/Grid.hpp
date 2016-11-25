#ifndef __GRID_HPP__
#define __GRID_HPP__

#include <vector>
#include <iostream>
#include <ngl/Vec2.h>


/// @file Grid.hpp
/// @brief The Grid class holds information about what is contained in each cell of the map.
/// The Grid class is a wrapper around a std::vector of Tile enums. The Tile enums illustrate
/// what is contained within each cell of the map, which can be used by other classes for
/// rendering and path finding.

/// @enum An enum to make identifying the cell type more obvious and easier to change
enum class Tile{
                ERROR, //returned when accessing non-existend tiles
                EMPTY, //empty tile
                FOREST, //forest tile
                BUILDING //building tile
               };

std::ostream& operator<<(std::ostream& _os, const Tile _t);

/// @class Grid
/// @brief A wrapper around a std::vector used to store map information
class Grid
{
public:

  /// @brief default ctor that sets the grid to a default 50 by 50 set of empty tiles and runs the initialiser
  Grid();

  /// @brief ctor that initialises a grid of empty tiles to a user specified size
  /// @param [in] _w user specified width of the grid
  /// @param [in] _h user specified height of the grid
  Grid(int _w, int _h);

  /// @brief ctor that initialises a grid of empty tiles to a user specified size
  /// @param [in] _w user specified width of the grid
  /// @param [in] _h user specified height of the grid
  Grid(int _w, int _h, std::string _script_path);

  /// @brief default dtor
  ~Grid() = default;

  /// @brief prints the map to the terminal
  void print();

  /// @brief returns the value of a tile at the specified width and height coordinate.
  /// It does not perform any error checking that the requested tile exists and will
  /// return garbage if the index is out of range.
  /// @param [in] _x is the x coordinate of the requested tile
  /// @param [in] _y is the y coordinate of the requested tile
  /// @return returns the Tile at the given coordinate
  Tile read(int _x, int _y);

  /// @brief returns the value of a tile at the specified coordinate.
  /// It does not perform any error checking that the requested tile exists and will
  /// return garbage if the index is out of range.
  /// @param [in] _coord is the coordinate as a ngl::Vec2
  /// @return returns the Tile at the given coordinate
  Tile read(ngl::Vec2 _coord );

  /// @brief returns the value of a tile at the specified width and height coordinate.
  /// It does not perform any error checking that the requested tile exists and will
  /// return garbage if the index is out of range.
  /// @param [in] _id is the 1d coordinate of the tile
  /// @return returns the Tile at the given coordinate
  Tile read(int _id);

  /// @brief sets the value of a tile at a given coordinate
  /// It does not perform any error checking that the requested tile exists.
  /// @param [in] _x is the x coordinate of the tile to be set
  /// @param [in] _y is the y coordinate of the tile to be set
  /// @param [in] _t is the value to be set at the given coordiate
  void write(int _x, int _y, Tile _t);

  /// @brief sets the value of a tile at a given coordinate
  /// It does not perform any error checking that the requested tile exists.
  /// @param [in] _coord is the coordinate as a ngl::Vec2
  /// @param [in] _t is the value to be set at the given coordiate
  void write(ngl::Vec2 _coord, Tile _t);

  /// @brief sets the value of a tile at a given coordinate
  /// It does not perform any error checking that the requested tile exists.
  /// @param [in] _id is the 1d coord of the tile
  /// @param [in] _t is the value to be set at the given coordiate
  void write(int _id, Tile _t);

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
  /// @brief puts down random set of circles of random terrain types
  /// @param [in] _max_rad is the maximum radius of the circles
  /// @param [in] _num_circles is the number of circles to be put down
  /// @param [in] _seed is the seed passed ot the random number generator
  void generateRandomMap(int _max_rad, int _num_circles, float _seed);

  /// @brief runs the given python script to generate a map
  /// @param [in] the script that will set values in m_map
  void runPyScript(std::string _script);

  /// @brief loads the script at the given path, returning the script in a single string
  /// @param [in] path to look for script
  /// @return a string containing the python script to be run
  std::string loadPyScript(std::string _script_path);

  /// @brief width of the grid
  int m_w;

  /// @brief height of the grid
  int m_h;

  /// @brief container for map information
  std::vector<Tile> m_map;
};

#endif//__GRID_HPP__
