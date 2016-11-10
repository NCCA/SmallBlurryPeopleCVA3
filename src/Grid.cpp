#include <iostream>
#include "Grid.hpp"


Grid::Grid():
  m_w(10),
  m_h(10),
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
      std::cout << (int)m_map[x + y * m_w];
    }
    std::cout << std::endl;
  }
}

void Grid::init()
{
  std::cout << "i made a grid! :D " << m_w << ", " << m_h << std::endl;

  for(Tile &t: m_map)
  {
    t = Tile::FOREST;
  }

  printMap();
}
