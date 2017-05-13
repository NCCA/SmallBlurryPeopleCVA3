#ifndef __MAPLIST_HPP__
#define __MAPLIST_HPP__

#include <vector>
#include <string>
#include "ngl/Singleton.h"

class MapList : public ngl::Singleton<MapList>
{
  friend class Singleton<MapList>;
public:
  MapList();
  std::string getCurrentMapPath();
  void nextMap();
  void prevMap();
  void addWidth(int x);
  void addHeight(int x);
  void addSeed(int x);

  int getW();
  int getH();
  int getSeed();

  std::string getWString();
  std::string getHString();
  std::string getSeedString();
private:
  std::vector<std::string> m_map_names;
  int m_current_map;
  int m_w;
  int m_h;
  int m_seed;
};

#endif//__MAPLIST_HPP__
