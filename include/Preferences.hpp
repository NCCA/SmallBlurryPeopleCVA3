#ifndef __PREFERENCES_HPP__
#define __PREFERENCES_HPP__

#include <string>
#include "ngl/Singleton.h"

class Preferences : public ngl::Singleton<Preferences>
{
  friend class Singleton<Preferences>;
public:
  void init();
  void save();
  int getXRes();
  int getYRes();
  bool getDOP();
  bool getShadows();
  bool getAA();
  bool getReflections();
  float getTimeScale();
  int getShadowMapRes();
  int getWaterMapRes();
  float getCharacterSpeed();
  std::string getMapScriptPath();

private:
  Preferences() = default;
  void parseFile(std::string _file_name);

  void parseXRes(const char *_begin);
  int m_x_res = 1920;

  void parseYRes(const char *_begin);
  int m_y_res = 1080;

  void parseDOP(const char *_begin);
  bool m_dop = true;

  void parseShadows(const char* _begin);
  bool m_shadows = true;

  void parseAA(const char* _begin);
  bool m_aa = true;

  void parseReflections(const char* _begin);
  bool m_reflections = true;

  void parseTimeScale(const char* _begin);
  float m_time_scale = 0.01;

  void parseShadowMapRes(const char* _begin);
  float m_shadow_map_res = 4096;

  void parseWaterMapRes(const char* _begin);
  float m_water_map_res = 1024;

  void parseCharacterSpeed(const char* _begin);
  float m_character_speed = 0.1;

  void parseMapScriptPath(const char *_begin);
  std::string m_map_script_path = "python/simplexMap.py";
};

#endif//__PREFERENCES_HPP__



