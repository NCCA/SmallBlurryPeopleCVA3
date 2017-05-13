#ifndef __MAPLIST_HPP__
#define __MAPLIST_HPP__

#include <vector>
#include <string>
#include "ngl/Singleton.h"
///
/// \file MapList.hpp contains MapList class
/// \author Felix

///
/// \brief The MapList class
///
class MapList : public ngl::Singleton<MapList>
{
  friend class Singleton<MapList>;
public:
  ///
  /// \brief MapList constructor sets up map variables
  ///
  MapList();
  ///
  /// \brief getCurrentMapName get name of map
  /// \return name of map as string
  ///
  std::string getCurrentMapName();
  ///
  /// \brief getCurrentMapPath get path of map
  /// \return path of map as string
  ///
  std::string getCurrentMapPath();
  ///
  /// \brief nextMap select the next map
  ///
  void nextMap();
  ///
  /// \brief prevMap select the previous map
  ///
  void prevMap();
  ///
  /// \brief addWidth change the width of the map
  /// \param x value to change width by
  ///
  void addWidth(int x);
  ///
  /// \brief addHeight change the height of the map
  /// \param x value to change height by
  ///
  void addHeight(int x);
  ///
  /// \brief addSeedchange the seed of the map
  /// \param x value to change seed by
  ///
  void addSeed(int x);
  ///
  /// \brief getW get map width
  /// \return m_w integer map width
  ///
  int getW();
  ///
  /// \brief getH get map height
  /// \return m_H integer map height
  ///
  int getH();
  ///
  /// \brief getSeed get map seed
  /// \return m_seed integer map seed
  ///
  int getSeed();
  ///
  /// \brief getWString get width as a string
  /// \return string containing "Width: "+m_w
  ///
  std::string getWString();
  ///
  /// \brief getHString get height as a string
  /// \return string containing "Height: "+m_h
  ///
  std::string getHString();
  ///
  /// \brief getSeedString get seed as a string
  /// \return string containing "Seed: "+m_seed
  ///
  std::string getSeedString();
private:
  ///
  /// \brief m_map_names vector of available maps
  ///
  std::vector<std::string> m_map_names;
  ///
  /// \brief m_current_map current map selected (integer id)
  ///
  int m_current_map;
  ///
  /// \brief m_w width of map, int
  ///
  int m_w;
  ///
  /// \brief m_h height of map, int
  ///
  int m_h;
  ///
  /// \brief m_seed map seed, int
  ///
  int m_seed;
};

#endif//__MAPLIST_HPP__
