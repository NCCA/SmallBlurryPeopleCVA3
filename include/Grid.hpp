#ifndef __GRID_HPP__
#define __GRID_HPP__

#include <vector>
#include <ngl/Vec2.h>
#include <ngl/Vec3.h>
#include "GridTile.hpp"

///
/// @file Grid.hpp
/// @brief header file for the Grid class
///

///
/// @class Grid
/// @brief The Grid class holds information about what is contained in each cell of the map.
/// The Grid class is a wrapper around a std::vector of Tile enums. The Tile enums illustrate
/// what is contained within each cell of the map, which can be used by other classes for
/// rendering and path finding.
///
class Grid
{
public:

  ///
  /// @brief default ctor that sets the grid to a default 50 by 50 set of empty tiles and runs the initialiser
  ///
  Grid();

  ///
  /// @brief updateScript loads the specified script and runs it to create a new map
  /// @param _script_path is the file path to the python script
  /// @param _new_w is the width of the map to generate
  /// @param _new_h is the new height of the map
  /// @param _new_seed is the seed value for random number generation in the map script
  ///
  void updateScript(std::string _script_path, int _new_w = 100, int _new_h = 100, int _new_seed = 8);


  void printTrees();
  void printTypes();
  ///
  /// \brief getTileType gets the tile type at the requested coordinates
  /// \param _x is the x component of the coordinate
  /// \param _y is the y component of the coordinate
  /// \return the TileType of the tile at position _x, _y
  ///
  TileType getTileType(int _x, int _y);

  ///
  /// \brief getTileType gets the tile type from a ngl::Vec2
  /// \param _coord the coordinate as Vec2
  /// \return the TileType at the given position
  ///
  TileType getTileType(ngl::Vec2 _coord);

  ///
  /// \brief getTileType gets the tile type at the given tile id
  /// \param _id is the id of the tile (aka its position in the 2D tile array)
  /// \return the TileType at the given id
  ///
  TileType getTileType(int _id);

  ///
  /// \brief getTileHeight gets the tile height at the given position
  /// \param _x is the x component of the position
  /// \param _y is the y component of the position
  /// \return the height value of the tile
  ///
  int getTileHeight(int _x, int _y);

  ///
  /// \brief getTileHeight gets the tile height using an integer tile id
  /// \param _id is the id of the tile (aka its position in the 1D tile array)
  /// \return the height value of the tile
  ///
  int getTileHeight(int _id);

  ///
  /// \brief getInterpolatedHeight interpolates the height between tiles at integer coordinates
  /// \param _x is the x component for the interpolated height
  /// \param _y is the y component for the interpolated height
  /// \return the height value interpolated from the 4 surrounding tiles
  ///
  float getInterpolatedHeight(float _x, float _y);

  ///
  /// \brief isTileTraversable returns a bool that indicated if a character can walk across this tile
  /// \param _x is the x coordinate of the tile of interest
  /// \param _y is the y coordinate of the tile of interest
  /// \return a bool value that is true if a character can walk on the tile
  ///
  bool isTileTraversable(int _x, int _y);

  ///
  /// \brief isTileTraversable returns a bool that indicated if a character can walk across this tile
  /// \param _id is the id of the tile of interest (aka its position in the 1 array of tiles)
  /// \return a bool value that is true if a character can walk on the tile
  ///
  bool isTileTraversable(int _id);

  ///
  /// \brief getTreePositions returns a vector of tree positions on the tile
  /// \param _x is the x coordinate of the tile of interest
  /// \param _y is the y coordinate of the tile of interest
  /// \return a vector of ngl::Vec2, one for each tree in the tile
  ///
  std::vector<ngl::Vec2> getTreePositions(int _x, int _y);

  ///
  /// \brief cutTileTrees reduces the amount of trees on the tile by the requested amount, with a limit of 0
  /// when the tile reaches 0 trees it is converted from a tree tile to an empty tile
  /// \param _id is the id of the tile of interest (aka its position in the 1D array of tiles
  /// \param _goal_amount is the desired number of trees to cut
  /// \return the number of trees cut, not necessarily the goal amount in cases where the tile has no trees left
  ///
  int cutTileTrees(int _id, int _goal_amount);

  ///
  /// \brief setTileType sets the tile type at the given id
  /// \param _id is the id of the tile of interest (aka its position in the 1D array of tiles)
  /// \param _type the new type of the tile
  ///
  void setTileType(int _id, TileType _type);

  ///
  /// \brief setTileType sets the tile type at the given coordinate
  /// \param _x is the x coordinate of the tile of interest
  /// \param _y is the y coordinate of the tile of interest
  /// \param _type is the new type of the tile
  ///
  void setTileType(int _x, int _y, TileType _type);

  ///
  /// \brief addBuildState adds to the build state of the tile, which represents the stage of completion of the building
  /// \param _id is id of the tile of interest
  /// \param _value the increment value of the build state
  /// \param _type the type of building being built
  ///
  void addBuildState(int _id, float _value, TileType _type);

  ///
  /// \brief addBuildState adds to the build state of the tile, which represents the stage of completion of the building
  /// \param _x is the x component of the tile of interest
  /// \param _y is the y component of the tile of interest
  /// \param _value is the value to increase the tiles build state by
  /// \param _type is the type of building being built
  ///
  void addBuildState(int _x, int _y, float _value, TileType _type);

  ///
  /// \brief getBuildState gets the build state of the tile with the given id
  /// \param _id is the id of the tile of interest
  /// \return the build state [0 ... 1] of the tile
  ///
  float getBuildState(int _id);

  ///
  /// \brief getBuildState gets the build state of the tile with the given id
  /// \param _x is the x coordinate of the tile of interest
  /// \param _y is the y coordinate of the tile of interest
  /// \return
  ///
  float getBuildState(int _x, int _y);

  ///
  /// \brief getNumTrees gets the number of trees at the tile
  /// \param _x is the x coordinate of the tile of interest
  /// \param _y is the y coordinate of the tile of interest
  /// \return the number of trees remaining on the tile
  ///
  int getNumTrees(int _x, int _y);

  ///
  /// \brief houseAdded increment number of houses in scene
  ///
  void houseAdded() {m_num_houses++;}
  ///
  /// \brief houseDestroyed decrement number of houses in scene
  ///
  void houseDestroyed() {m_num_houses--;}

  ///
  /// \brief getNumHouses get the number of houses on the grid
  /// \return m_num_houses, number of houses on grid
  ///
  int getNumHouses() {return m_num_houses;}

  /// @brief converts a tile id to a coordinate, the tile id is the
  /// one dimensional coordinate of the tile
  /// @param the tile id to convert
  /// @return a ngl::vec2 that is the 2d coordinate of the tile
  ///
  ngl::Vec2 idToCoord(int _tileId);

  ///
  /// @brief converts a coordinate to a tile id
  /// @param the tile id to convert
  /// @return a ngl::vec2 that is the 2d coordinate of the tile
  ///
  int coordToId(ngl::Vec2 _coord);

  ///
  /// @brief returns grid width
  /// @return int width of grid
  ///
  int getW();

  ///
  /// @brief returns grid height
  /// @return int height of grid
  ///
  int getH();

  ///
  /// \brief getGlobalMountainHeight returns the height limit that divides mountains from normal terrain
  /// set in the python script
  /// \return the height of the mountains
  ///
  int getGlobalMountainHeight();

  ///
  /// \brief getGlobalWaterLevel returns the water level, set in the python script
  /// \return
  ///
  int getGlobalWaterLevel();

  ///
  /// \brief hasChanges returns a boolean flag that indicated if changes have been made to the grid.
  /// changes include cutting down trees and building new houses, this is used to re-build the
  /// mesh instances
  /// \return
  ///
  bool hasChanges();

  ///
  /// \brief resetHasChanges resets the has changes flag to false
  ///
  void resetHasChanges();

  ///
  /// \brief getSpawnPoint retrieves the spawn point set by the python script
  /// \return the spwan point as an ngl::Vec2
  ///
  ngl::Vec2 getSpawnPoint();

  ///
  /// \brief checkID checks that the id asked for is valid
  /// \param _id the id to check
  /// \return true if the id is valid
  ///
  bool checkID(int _id);

  ///
  /// \brief checkCoord checks that the coordinate asked for is valid
  /// \param _x is the x coordinate
  /// \param _y is the y coordinate
  /// \return true if the coordinate is valid
  ///
  bool checkCoord(double _x, double _y);

  ///
  /// \brief checkCoord checks that the coordinate asked for is valid
  /// \param _p the coordinate as an ngl vec2
  /// \return true if the coordinate is valid
  ///
  bool checkCoord(ngl::Vec2 _p);

private:
  ///
  /// \brief runCurrentScript runs the currently sotred python script to generate a map
  /// \param _w the width of the map to generate with the script
  /// \param _h the height of the map to generate with the script
  /// \param _seed the seed value to be given to the radom number generators in the script
  ///
  void runCurrentScript(int _w, int _h, int _seed);

  ///
  /// \brief loadScript loads the script at the given path, returning the script in a single string
  /// \param _script_path the path (from the directory where the game is run) to they python script
  ///
  void loadScript(std::string _script_path);

  ///
  /// @brief width of the grid
  ///
  int m_w;

  ///
  /// @brief height of the grid
  ///
  int m_h;

  ///
  /// @brief container for map information
  ///
  std::vector<GridTile> m_tiles;

  ///
  /// \brief m_script holds the python script as a single string
  ///
  std::string m_script;

  ///
  /// \brief m_mountain_height is a value that determines the boundary value beween normal tiles and mountains
  ///
  int m_mountain_height = 0;

  ///
  /// \brief m_water_level is the waterlevel for the map
  ///
  int m_water_level = 0;

  ///
  /// \brief m_has_changes a flag that indicates if changes have been made to the grid that means the mesh
  /// instances need to be rebuilt
  ///
  bool m_has_changes = false;

  ///
  /// \brief m_num_houses number of houses on the map
  ///
  int m_num_houses;

  ///
  /// \brief m_num_wood amount of wood stored
  ///
  int m_num_wood;

  ///
  /// \brief m_num_berries amount of berries stored
  ///
  int m_num_berries;

  ///
  /// \brief m_num_fish amount of fish stored
  ///
  int m_num_fish;

  ///
  /// \brief m_spawn_ponit is the spawn point set in the python script
  ///
  ngl::Vec2 m_spawn_ponit;
};

#endif//__GRID_HPP__
