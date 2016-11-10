#ifndef __GRID_HPP__
#define __GRID_HPP__

#include <vector>

/// @file Grid.hpp
/// @brief The Grid class holds information about what is contained in each cell of the map.
/// The Grid class is a wrapper around a std::vector of Tile enums. The Tile enums illustrate
/// what is contained within each cell of the map, which can be used by other classes for
/// rendering and path finding.

/// @enum An enum to make identifying the cell type more obvious and easier to change
enum class Tile{EMPTY, FOREST, BUILDING};

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

  /// @brief default dtor
  ~Grid() = default;

  /// @brief prints the map to the terminal
  void printMap();

  /// @brief returns the value of a tile at the specified width and height coordinate.
  /// It does not perform any error checking that the requested tile exists and will
  /// return garbage if the index is out of range.
  /// @param [in] _x is the x coordinate of the requested tile
  /// @param [in] _y is the y coordinate of the requested tile
  /// @param [out] returns the Tile at the given coordinate
  Tile read(int _x, int _y);

  /// @brief sets the value of a tile at a given coordinate
  /// It does not perform any error checking that the requested tile exists.
  /// @param [in] _x is the x coordinate of the tile to be set
  /// @param [in] _y is the y coordinate of the tile to be set
  /// @param [in] _t is the value to be set at the given coordiate
  void write(int _x, int _y, Tile _t);

  /// @brief puts down random set of circles of random terrain types
  /// @param [in] _max_rad is the maximum radius of the circles
  /// @param [in] _num_circles is the number of circles to be put down
  /// @param [in] _seed is the seed passed ot the random number generator
  void generateRandomMap(int _max_rad, int _num_circles, float _seed);

private:
  /// @brief initilaiser for the class that should be called by the constructors
  void init();

  /// @brief width of the grid
  int m_w;

  /// @brief height if the grid
  int m_h;

  /// @brief container for map information
  std::vector<Tile> m_map;
};

#endif//__GRID_HPP__
