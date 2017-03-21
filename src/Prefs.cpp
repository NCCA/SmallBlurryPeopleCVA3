#include <iostream>
#include <iomanip>
#include <fstream>

#include "Prefs.hpp"
#include "PrefsParser.hpp"

void Prefs::init(std::string _pref_file)
{
  PrefsParser parser;
  parser.parseFile(_pref_file);
}

void Prefs::restoreDefaultPrefs()
{
  //clearing all currently set prefs
  m_int_prefs.clear();
  m_float_prefs.clear();
  m_str_prefs.clear();

  //setting new values
  m_int_prefs["AA"] = 1;
  m_int_prefs["DOP"] = 1;
  m_int_prefs["REFLECTIONS"] = 1;
  m_int_prefs["SHADOWS"] = 1;
  m_int_prefs["SHADOW_MAP_RES"] = 4096;
  m_int_prefs["WATER_MAP_RES"] = 1024;
  m_int_prefs["X_RES"] = 1920;
  m_int_prefs["Y_RES"] = 1080;

  m_float_prefs["CHARACTER_SPEED"] = 0.1;
  m_float_prefs["TIME_SCALE"] = 0.01;

  m_str_prefs["MAP_SCRIPT_PATH"] = "python/simplexMap.py";
}

void Prefs::setIntPref(std::string _key, int _val)
{
  m_int_prefs[_key] = _val;
}

void Prefs::setFloatPref(std::string _key, float _val)
{
  m_float_prefs[_key] = _val;
}

void Prefs::setStrPref(std::string _key, std::string _val)
{
  m_str_prefs[_key] = _val;
}

int Prefs::getIntPref(std::string _key)
{
  return m_int_prefs[_key];
}

float Prefs::getFloatPref(std::string _key)
{
  return m_float_prefs[_key];
}

std::string Prefs::getStrPref(std::string _key)
{
  return m_str_prefs[_key];
}

void Prefs::printPrefs()
{
  for (auto i: m_int_prefs)
  {
    std::cout << i.first << ", " << i.second << std::endl;
  }
  for (auto i: m_float_prefs)
  {
    std::cout << i.first << ", " << i.second << std::endl;
  }
  for (auto i: m_str_prefs)
  {
    std::cout << i.first << ", " << i.second << std::endl;
  }
}

void Prefs::savePrefs()
{
  std::ofstream preferences;
  preferences.open("preferences2.conf");
  for (auto i: m_int_prefs)
  {
    preferences << i.first << " = " << i.second << "\n";
  }
  for (auto i: m_float_prefs)
  {
    preferences << i.first << " = " << std::fixed << std::setprecision(4) << float(i.second) << "\n";
  }
  for (auto i: m_str_prefs)
  {
    preferences << i.first << " = \"" << i.second << "\"\n";
  }
}
