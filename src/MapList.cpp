#include "MapList.hpp"
#include "Prefs.hpp"
#include "Utility.hpp"
#include <iostream>

MapList::MapList() :
  m_current_map(0),
  m_w(50),
  m_h(50),
  m_seed(0)
{
  m_map_names.push_back("arena");
  m_map_names.push_back("connectedDots");
  m_map_names.push_back("simplexMap");
  m_map_names.push_back("survivalIsland");
  m_map_names.push_back("uk");
}

std::string MapList::getCurrentMapPath()
{
  if((size_t)m_current_map<m_map_names.size())
  {
    return m_map_names[m_current_map];
  }
  else if((size_t)m_current_map == m_map_names.size())
  {
    return "Custom: "+Prefs::instance()->getStrPref("MAP_SCRIPT");
  }
  else
  {
    return "";
  }
}

void MapList::nextMap()
{
  m_current_map++;
  m_current_map %= m_map_names.size()+1;
  std::cout << "next, now:" << m_current_map << std::endl;
}

void MapList::prevMap()
{
  m_current_map--;
  if(m_current_map<0)
  {
    m_current_map += m_map_names.size()+1;
  }
  std::cout << "prev, now:" << m_current_map << std::endl;
}

void MapList::addWidth(int x)
{
  m_w+=x;
  m_w = Utility::clamp(m_w, 30, 500);
}

void MapList::addHeight(int x)
{
  m_h+=x;
  m_h = Utility::clamp(m_h, 30, 500);
}

void MapList::addSeed(int x)
{
  m_seed+=x;
  m_seed = Utility::clamp(m_seed, 0, 999);
}

int MapList::getW()
{
  return m_w;
}

int MapList::getH()
{
  return m_h;
}

int MapList::getSeed()
{
  return m_seed;
}

std::string MapList::getWString()
{
  return "Width: "+std::to_string(m_w);
}

std::string MapList::getHString()
{
  return "Height: "+std::to_string(m_h);
}

std::string MapList::getSeedString()
{
  return "Seed: "+std::to_string(m_seed);
}
