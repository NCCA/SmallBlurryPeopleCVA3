#include <iostream>
#include "ngl/Random.h"
#include "Grid.hpp"


/// @file Grid.cpp
/// @brief source code for the Grid class

Grid::Grid():
  m_w(50),
  m_h(50),
  m_map(m_w * m_h, Tile::EMPTY)
{
  init();
}

Grid::Grid(int _w, int _h):
  m_w(_w),
  m_h(_h),
  m_map(m_w * m_h, Tile::EMPTY)
{
  init();
}

void Grid::printMap()
{
  for(int x = 0; x < m_w; x++)
  {
    for(int y = 0; y < m_h; y++)
    {
      std::cout << (int)read(x, y);
    }
    std::cout << std::endl;
  }
}

Tile Grid::read(int _x, int _y)
{
  return m_map[_x + _y * m_w];
}

void Grid::write(int _x, int _y, Tile _t)
{
  m_map[_x + _y * m_w] = _t;
}

ngl::Vec2 Grid::idToCoord(int _tileId)
{
  int x = _tileId % m_w;
  int y = _tileId / m_w;
  return ngl::Vec2(x, y);
}

int Grid::coordToId(ngl::Vec2 _coord)
{
  return _coord.m_x + m_w * _coord.m_y;
}

void Grid::generateRandomMap(int _max_rad, int _num_circles, float _seed)
{
  //init the ngl random number generator with the current time
  ngl::Random *random_engine = ngl::Random::instance();
  random_engine->setSeed(_seed);

  //generate 5 random regions
  for (int i = 0; i < _num_circles; i++)
  {
    //generate 4 random numbers: type, x, y and radius to describe the circle to be drawn
    int x_rand = (int)random_engine->randomPositiveNumber(m_w);
    int y_rand = (int)random_engine->randomPositiveNumber(m_h);
    int r_rand = (int)random_engine->randomPositiveNumber(_max_rad);
    Tile t_rand = (Tile)random_engine->randomPositiveNumber(3);
    //now draw the circle
    for (int x = - r_rand; x < r_rand; x++)
    {
      for (int y = - r_rand; y < r_rand; y++)
      {
        if (x * x + y * y < r_rand * r_rand)
        {
          int x_out = x + x_rand;
          int y_out = y + y_rand;
          if (x_out >= 0 && x_out < m_w && y_out >= 0 && y_out < m_h)
          {
            write(x + x_rand, y + y_rand, t_rand);
          }
        }
      }
    }
  }
}


void Grid::init()
{
  std::cout << "i made a grid! :D " << m_w << ", " << m_h << std::endl;

  generateRandomMap(20, 10, 5);
  printMap();
}
