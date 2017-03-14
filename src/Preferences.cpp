#include <iostream>
#include <fstream>
#include <boost/bind.hpp>
#include <boost/spirit/include/classic.hpp>
#include "Preferences.hpp"

namespace spt = boost::spirit::classic;
typedef spt::rule<spt::phrase_scanner_t> srule;


void Preferences::init()
{
  parseFile("preferences.conf");
}

void Preferences::parseFile(std::string _file_name)
{
  srule comment = spt::comment_p("#");
  srule map_script_path = ("MAP_SCRIPT_PATH:" >> *(spt::anychar_p))
                     [bind(&Preferences::parseMapScriptPath, boost::ref(*this), _1)];
  srule x_res = ("X_RES" >> *(spt::anychar_p))
                [bind(&Preferences::parseXRes, boost::ref(*this), _1)];
  srule y_res = ("Y_RES" >> *(spt::anychar_p))
                [bind(&Preferences::parseYRes, boost::ref(*this), _1)];
  srule dop = ("DOP" >> *(spt::anychar_p))
              [bind(&Preferences::parseDOP, boost::ref(*this), _1)];
  srule shadows = ("SHADOWS" >> *(spt::anychar_p))
                  [bind(&Preferences::parseShadows, boost::ref(*this), _1)];
  srule aa = ("AA" >> *(spt::anychar_p))
                  [bind(&Preferences::parseAA, boost::ref(*this), _1)];
  srule reflections = ("REFLECTIONS" >> *(spt::anychar_p))
                      [bind(&Preferences::parseReflections, boost::ref(*this), _1)];
  srule time_scale = ("TIME_SCALE" >> *(spt::anychar_p))
      [bind(&Preferences::parseTimeScale, boost::ref(*this), _1)];
  srule shadow_map_res = ("SHADOW_MAP_RES" >> *(spt::anychar_p))
      [bind(&Preferences::parseShadowMapRes, boost::ref(*this), _1)];
  srule water_map_res = ("WATER_MAP_RES" >> *(spt::anychar_p))
      [bind(&Preferences::parseWaterMapRes, boost::ref(*this), _1)];
  srule character_speed = ("CHARACTER_SPEED" >> *(spt::anychar_p))
      [bind(&Preferences::parseCharacterSpeed, boost::ref(*this), _1)];

  std::ifstream fileIn(_file_name.c_str());
  if (fileIn.is_open()){
    std::string line;
    while (std::getline(fileIn, line)){
      // parsing the line according to the inital rules set above
      spt::parse(line.c_str(),
                 comment |
                 map_script_path |
                 x_res |
                 y_res |
                 dop|
                 shadows |
                 aa |
                 reflections |
                 time_scale |
                 shadow_map_res |
                 water_map_res |
                 character_speed,
                 spt::space_p);
    }
  }
}

void Preferences::save()
{
  std::ofstream preferences;
  preferences.open("preferences.conf");
  preferences << "X_RES: " << m_x_res << "\n";
  preferences << "Y_RES: " << m_y_res << "\n";
  preferences << "DOP: " << (int)m_dop << "\n";
  preferences << "SHADOWS: " << (int)m_shadows << "\n";
  preferences << "AA: " << (int)m_aa << "\n";
  preferences << "REFLECTIONS: " << (int)m_reflections << "\n";
  preferences << "TIME_SCALE: " << m_time_scale << "\n";
  preferences << "SHADOW_MAP_RES: " << m_shadow_map_res << "\n";
  preferences << "WATER_MAP_RES: " << m_water_map_res << "\n";
  preferences << "CHARACTER_SPEED: " << m_character_speed << "\n";
  preferences << "MAP_SCRIPT_PATH: " << m_map_script_path << "\n";
  preferences.close();
}

void Preferences::parseXRes(const char *_begin)
{
  srule x_res = "X_RES:" >>
                spt::int_p[spt::assign_a(m_x_res)];
  parse(_begin, x_res, spt::space_p);
}

int Preferences::getXRes()
{
  return m_x_res;
}

void Preferences::parseYRes(const char *_begin)
{
  srule y_res = "Y_RES:" >>
                spt::int_p[spt::assign_a(m_y_res)];
  parse(_begin, y_res, spt::space_p);
}

int Preferences::getYRes()
{
  return m_y_res;
}

void Preferences::parseMapScriptPath(const char *_begin)
{
  srule map_script_path = "MAP_SCRIPT_PATH:" >>
                          (*(+spt::alnum_p >> "/") >>
                          +spt::alnum_p >> ".py")[spt::assign_a(m_map_script_path)];
  parse(_begin, map_script_path, spt::space_p);
}

std::string Preferences::getMapScriptPath()
{
  return m_map_script_path;
}

void Preferences::parseDOP(const char *_begin)
{
  srule dop = "DOP:" >>
                spt::int_p[spt::assign_a(m_dop)];
  parse(_begin, dop, spt::space_p);
}

bool Preferences::getDOP()
{
  return m_dop;
}

void Preferences::parseShadows(const char *_begin)
{
  srule shadows = "SHADOWS:" >>
                spt::int_p[spt::assign_a(m_shadows)];
  parse(_begin, shadows, spt::space_p);
}

bool Preferences::getShadows()
{
  return m_shadows;
}

void Preferences::parseAA(const char *_begin)
{
  srule rule = "AA:" >>
                spt::int_p[spt::assign_a(m_aa)];
  parse(_begin, rule, spt::space_p);
}

bool Preferences::getAA()
{
  return m_aa;
}

void Preferences::parseReflections(const char *_begin)
{
  srule rule = "REFLECTIONS:" >>
                spt::int_p[spt::assign_a(m_reflections)];
  parse(_begin, rule, spt::space_p);
}

bool Preferences::getReflections()
{
  return m_reflections;
}

void Preferences::parseTimeScale(const char *_begin)
{
  srule rule = "TIME_SCALE:" >>
                spt::real_p[spt::assign_a(m_time_scale)];
  parse(_begin, rule, spt::space_p);
}

float Preferences::getTimeScale()
{
  return m_time_scale;
}

void Preferences::parseShadowMapRes(const char *_begin)
{
  srule rule = "SHADOW_MAP_RES:" >>
                spt::int_p[spt::assign_a(m_shadow_map_res)];
  parse(_begin, rule, spt::space_p);
}

int Preferences::getShadowMapRes()
{
  return m_shadow_map_res;
}

void Preferences::parseWaterMapRes(const char *_begin)
{
  srule rule = "WATER_MAP_RES:" >>
                spt::int_p[spt::assign_a(m_water_map_res)];
  parse(_begin, rule, spt::space_p);
}

int Preferences::getWaterMapRes()
{
  return m_water_map_res;
}

void Preferences::parseCharacterSpeed(const char *_begin)
{
  srule rule = "CHARACTER_SPEED:" >>
                spt::real_p[spt::assign_a(m_character_speed)];
  parse(_begin, rule, spt::space_p);
}

float Preferences::getCharacterSpeed()
{
  return m_character_speed;
}
