#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <streambuf>
#include <Python.h>
#include "ngl/Random.h"
#include "ngl/NGLStream.h"
#include "Grid.hpp"
#include "Prefs.hpp"
#include "Utility.hpp"
#include "Prefs.hpp"

/// @file Grid.cpp
/// @brief source code for the Grid class

Grid::Grid():
  m_w(1),
  m_h(1),
  m_num_houses(0)
{
  Prefs* prefs = Prefs::instance();
  updateScript(prefs->getStrPref("MAP_SCRIPT_PATH"));
}

void Grid::updateScript(std::string _script_path, int _new_w, int _new_h, int _new_seed)
{
  loadScript(_script_path);
  runCurrentScript(_new_w, _new_h, _new_seed);
}

void Grid::runCurrentScript(int _w, int _h, int _seed)
{
  // init python stuff
  Py_Initialize();

  //create pyobjects to represent the python variables
  //py_main represents the python script to be imported
  PyObject *py_main = PyImport_ImportModule("__main__");
  //py_dict is a dictionary of global variables in the script
  PyObject *py_dict = PyModule_GetDict(py_main);
  //py_map is a bridge object between m_map and the python map
  PyObject *py_map;

  //passing values from the TileType enum class to the python script
  PyObject *py_tileTypes = PyDict_New();
  PyDict_SetItemString(py_tileTypes, "NONE", PyInt_FromLong((long)TileType::NONE));
  PyDict_SetItemString(py_tileTypes, "TREES", PyInt_FromLong((long)TileType::TREES));
  PyDict_SetItemString(py_tileTypes, "WATER", PyInt_FromLong((long)TileType::WATER));
  PyDict_SetItemString(py_tileTypes, "MOUNTAINS", PyInt_FromLong((long)TileType::MOUNTAINS));
  PyDict_SetItemString(py_tileTypes, "HOUSE", PyInt_FromLong((long)TileType::HOUSE));
  PyDict_SetItemString(py_tileTypes, "STOREHOUSE", PyInt_FromLong((long)TileType::STOREHOUSE));
  PyDict_SetItemString(py_dict, "tileTypes_in", py_tileTypes);
  PyDict_SetItemString(py_dict, "seed_in", PyInt_FromLong(_seed));
  PyDict_SetItemString(py_dict, "width_in", PyInt_FromLong(_w));
  PyDict_SetItemString(py_dict, "height_in", PyInt_FromLong(_h));
  PyDict_SetItemString(py_dict, "from_game", PyBool_FromLong(1));
  //run the script held in the m_script string
  PyRun_SimpleString(m_script.c_str());

  //extract width and height and map data
  m_w = _w;
  m_h = _h;
  m_mountain_height = PyInt_AsLong(PyDict_GetItemString(py_dict, "mountain_height"));
  m_water_level = PyInt_AsLong(PyDict_GetItemString(py_dict, "water_height"));
  py_map = PyDict_GetItemString(py_dict, "map_data");
  m_spawn_ponit[0] = PyInt_AsLong(PyTuple_GetItem(PyDict_GetItemString(py_dict, "spawn_point"), 0));
  m_spawn_ponit[1] = PyInt_AsLong(PyTuple_GetItem(PyDict_GetItemString(py_dict, "spawn_point"), 1));
  std::cout << "spawn point is set to: " << m_spawn_ponit << std::endl;
 //create an empty grid with each tile's id set

  for (int i = 0; i < m_w * m_h; i++)
  {
    m_tiles.push_back(GridTile(i));
  }
  int max_trees = Prefs::instance()->getIntPref("TREES_PER_TILE");
  //get grid values from the script
  for(int i = 0; i <  m_w * m_h; i++)
  {
    TileType t = (TileType)PyInt_AsLong(PyList_GetItem(PyList_GetItem(py_map, i), 0));
    int height =  PyInt_AsLong(PyList_GetItem(PyList_GetItem(py_map, i), 1));
    m_tiles[i].setType(t);
    m_tiles[i].setHeight(height);
    std::cout << height << std::endl;
    if (t == TileType::TREES)
    {
      m_tiles[i].setNumTrees(max_trees);
    }
  }

  //now that all python operations have finished, I can un-initialize the python libraries
  Py_Finalize();
}

ngl::Vec2 Grid::idToCoord(int _tileId)
{
    int x = _tileId % m_w;
  int y = _tileId / m_w;
  return ngl::Vec2(x, y);
}

int Grid::coordToId(ngl::Vec2 _coord)
{
  return (int)(_coord.m_x + m_w * (int)_coord.m_y);
}

void Grid::loadScript(std::string _script_path)
{

  std::ifstream script_file(_script_path.c_str());
  if (!script_file.is_open())
  {
    //if the file is not found, an empty string is returned
    std::cerr << "error opening script: " << _script_path << std::endl;
    m_script =  std::string();
  }
  m_script = std::string((std::istreambuf_iterator<char>(script_file)),
             std::istreambuf_iterator<char>());
}

int Grid::getW()
{
  return m_w;
}

int Grid::getH()
{
  return m_h;
}

int Grid::getGlobalMountainHeight()
{
  return m_mountain_height;
}

int Grid::getGlobalWaterLevel()
{
  return m_water_level;
}

float Grid::getInterpolatedHeight(float _x, float _y)
{
  //clamping the input values to 0 and their respective limits
  //handles edge cases for the map
  float x0 = Utility::clamp(std::round(_x), 0, m_w-1);
  float x1 = Utility::clamp(x0 + 1, 0, m_w);
  float y0 = Utility::clamp(std::round(_y), 0, m_h-1);
  float y1 = Utility::clamp(y0 + 1, 0, m_h);

  //retrieving the 4 height values to interpolate between
  float h0 = m_tiles[x0 + m_w * y0].getHeight();
  float h1 = m_tiles[x1 + m_w * y0].getHeight();
  float h2 = m_tiles[x0 + m_w * y1].getHeight();
  float h3 = m_tiles[x1 + m_w * y1].getHeight();

  //interpolate in the x direction
  float h4 = h0 * (x1 - _x) + h1 * (_x - x0);
  float h5 = h2 * (x1 - _x) + h3 * (_x - x0);

  //interpolate in the y direction
  float h6 = h4 * (y1 - _y) + h5 * (_y - y0);

  return h6;
}

TileType Grid::getTileType(ngl::Vec2 _coord)
{
  return m_tiles[_coord.m_x + m_w * _coord.m_y].getType();
}

TileType Grid::getTileType(int _x, int _y)
{
  return m_tiles[_x + m_w * _y].getType();
}

TileType Grid::getTileType(int _id)
{
  return m_tiles[_id].getType();
}

int Grid::getTileHeight(int _x, int _y)
{
  return m_tiles[_x + m_w * _y].getHeight();
}

int Grid::getTileHeight(int _id)
{
  return m_tiles[_id].getHeight();
}

bool Grid::isTileTraversable(int _x, int _y)
{
  if (_x > 0 && _y > 0 && _x < m_w && _y < m_h)
  {
  return m_tiles[_x + m_w * _y].isTraversable();
  }
  else
  {
    return false;
  }
}

bool Grid::isTileTraversable(int _id)
{
  if (_id >= 0 && _id < m_w * m_h)
  {
    return m_tiles[_id].isTraversable();
  }
  else
  {
    return false;
  }
}

int Grid::cutTileTrees(int _id, int _goal_amount)
{
  int out =  m_tiles[_id].cutTrees(_goal_amount);
  m_has_changes = true;
  return out;
}

void Grid::setTileType(int _id, TileType _type)
{
  m_tiles[_id].setType(_type);
}

void Grid::setTileType(int _x, int _y, TileType _type)
{
  m_tiles[_x + m_w * _y].setType(_type);
}

void Grid::setBuildState(int _id, float _value, TileType _type)
{
  m_tiles[_id].setBuildState(_value, _type);
  if(_value >= 1.0 && _type == TileType::HOUSE)
  {
    houseAdded();
  }
  m_has_changes = true;
}

void Grid::setBuildState(int _x, int _y, float _value, TileType _type)
{
  m_tiles[_x + m_w * _y].setBuildState(_value, _type);
  if(_value >= 1.0 && _type == TileType::HOUSE)
  {
    houseAdded();
  }
  m_has_changes = true;
}

float Grid::getBuildState(int _id)
{
  return m_tiles[_id].getBuildState();
}

float Grid::getBuildState(int _x, int _y)
{
  return m_tiles[_x + m_w * _y].getBuildState();
}

bool Grid::hasChanges()
{
  return m_has_changes;
}

void Grid::resetHasChanges()
{
  m_has_changes = false;
}


std::vector<ngl::Vec2> Grid::getTreePositions(int _x, int _y)
{
  return m_tiles[_x + m_w * _y].getTreePositions();
}

int Grid::getNumTrees(int _x, int _y)
{
  return m_tiles[_x + m_w * _y].getNumTrees();
}

ngl::Vec2 Grid::getSpawnPoint()
{
  return m_spawn_ponit;
}

bool Grid::checkID(int _id)
{
  if (_id > 0 && _id > m_tiles.size())
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool Grid::checkCoord(double _x, double _y)
{
  if (_x > 0 && _y > 0 && _x < m_w && _y < m_h)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool Grid::checkCoord(ngl::Vec2 _p)
{
  if (_p[0] > 0 && _p[1] > 0 && _p[0] < m_w && _p[1] < m_h)
  {
    return true;
  }
  else
  {
    return false;
  }
}





