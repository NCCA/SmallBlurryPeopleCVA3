#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <Python.h>
#include "ngl/Random.h"
#include "Grid.hpp"


/// @file Grid.cpp
/// @brief source code for the Grid class


std::ostream& operator<<(std::ostream& _os, const Tile _t)
{
  switch(_t)
  {
    case(Tile::ERROR):    return _os << "ERROR   "; break;
    case(Tile::EMPTY):    return _os << "EMPTY   "; break;
    case(Tile::FOREST):   return _os << "FOREST  "; break;
    case(Tile::BUILDING): return _os << "BUILDING"; break;
    default:              return _os << "ERROR   "; break;
  }
}


Grid::Grid():
  m_w(1),
  m_h(1)
{
  updateScript("python/readImgMap.py");
  print2();
}

void Grid::updateScript(std::string _script_path)
{
  loadScript(_script_path);
  runCurrentScript();
}

void Grid::runCurrentScript()
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

  PyObject *py_tile_error = PyInt_FromLong((int)Tile::ERROR);
  PyObject *py_tile_empty = PyInt_FromLong((int)Tile::EMPTY);
  PyObject *py_tile_forest = PyInt_FromLong((int)Tile::FOREST);
  PyObject *py_tile_building = PyInt_FromLong((int)Tile::BUILDING);

  //passing data from the c++ program to the dictionary of global variables in the script
  PyDict_SetItemString(py_dict, "tile_error", py_tile_error);
  PyDict_SetItemString(py_dict, "tile_empty", py_tile_empty);
  PyDict_SetItemString(py_dict, "tile_forest", py_tile_forest);
  PyDict_SetItemString(py_dict, "tile_building", py_tile_building);

  //run the script
  PyRun_SimpleString(m_script.c_str());


  m_w = PyInt_AsLong(PyDict_GetItemString(py_dict, "map_width"));
  m_h = PyInt_AsLong(PyDict_GetItemString(py_dict, "map_height"));
  py_map = PyDict_GetItemString(py_dict, "map_data");
  m_map = std::vector<Tile>( m_w * m_h, Tile::EMPTY);//making stuff here, good progress !

  //get grid values from the script
  for(size_t i = 0; i < m_map.size(); i++)
  {
    m_map[i] = (Tile)PyInt_AsLong(PyList_GetItem(py_map, i));
  }

  //now that all python operations have finished, I can un-initialize the python libraries
  Py_Finalize();
}

void Grid::print()
{
  for(int y = 0; y < m_h; y++)
  {
    for(int x = 0; x < m_w; x++)
    {
      std::cout << read(x, y) << " ";
    }
    std::cout << std::endl;
  }
}

void Grid::print2()
{
  for(int y = 0; y < m_h; y++)
  {
    for(int x = 0; x < m_w; x++)
    {
      Tile t = read(x, y);
      switch(t)
      {
        case(Tile::ERROR): std::cout << "E "; break;
        case(Tile::BUILDING): std::cout << "X "; break;
        case(Tile::EMPTY): std::cout << "  "; break;
        case(Tile::FOREST): std::cout << "X "; break;
      }
    }
    std::cout << std::endl;
  }
}

Tile Grid::read(int _x, int _y)
{
  if (_x >= 0 &&
      _y >= 0 &&
      _x < m_w &&
      _y < m_h)
  {
    return m_map[_x + _y * m_w];
  }
  else
  {
    return Tile::ERROR;
  }
}

Tile Grid::read(ngl::Vec2 _coord)
{
  if (_coord.m_x >= 0 &&
      _coord.m_y >= 0 &&
      _coord.m_x < m_w &&
      _coord.m_y < m_h)
  {
    return m_map[_coord.m_x + _coord.m_y * m_w];
  }
  else
  {
    return Tile::ERROR;
  }
}

Tile Grid::read(int _id)
{
  if (_id >= 0 &&
      (size_t)_id < m_map.size())
  {
    return m_map[_id];
  }
  else
  {
    return Tile::ERROR;
  }
}

void Grid::write(int _x, int _y, Tile _t)
{
  if (_x >= 0 &&
      _y >= 0 &&
      _x < m_w &&
      _y < m_h)
  {
    m_map[_x + _y * m_w] = _t;
  }
  else
  {
    std::cerr << "ERROR: Attemtping to write tile out of range\n";
  }
}

void Grid::write(ngl::Vec2 _coord, Tile _t)
{
  if (_coord.m_x >= 0 &&
      _coord.m_y >= 0 &&
      _coord.m_x < m_w &&
      _coord.m_y < m_h)
  {
    m_map[_coord.m_x + _coord.m_y * m_w] = _t;
  }
  else
  {
    std::cerr <<"ERROR: Attemting to write tile out of range\n";
  }
}

void Grid::write(int _id, Tile _t)
{
  if (_id >= 0 &&
      (size_t)_id < m_map.size())
  {
    m_map[_id] = _t;
  }
  else
  {
    std::cerr << "ERROR: Attemting to write tile out of range\n";
  }
}

ngl::Vec2 Grid::idToCoord(int _tileId)
{
  int x = _tileId % m_w;
  int y = _tileId / m_w;
  return ngl::Vec2(x, y);
}

int Grid::coordToId(ngl::Vec2 _coord)
{
  int x = (int)_coord.m_x;
  int y = (int)_coord.m_y;
  return _coord.m_x + m_w * _coord.m_y;
}

void Grid::resize(int _w, int _h)
{
  m_w = _w;
  m_h = _h;
  m_map = std::vector<Tile>(m_w * m_h, Tile::EMPTY);
}

void Grid::loadScript(std::string _script_path)
{

  std::ifstream script_file(_script_path.c_str());
  if (!script_file.is_open())
  {
    //if the file is not found, an empty string is returned
    std::cerr << "file not found " << _script_path << std::endl;
    m_script =  std::string();
  }
  m_script = std::string((std::istreambuf_iterator<char>(script_file)),
             std::istreambuf_iterator<char>());
}




