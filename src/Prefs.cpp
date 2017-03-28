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
  //setting int values
  m_int_prefs["AA"] = 1;
  m_int_prefs["DOP"] = 1;
  m_int_prefs["REFLECTIONS"] = 1;
  m_int_prefs["SHADOWS"] = 1;
  m_int_prefs["SHADOW_MAP_RES"] = 4096;
  m_int_prefs["WATER_MAP_RES"] = 1024;
  m_int_prefs["X_RES"] = 1920;
  m_int_prefs["Y_RES"] = 1080;

  //setting float values
  m_float_prefs["CHARACTER_SPEED"] = 0.1;
  m_float_prefs["TIME_SCALE"] = 0.01;

  //setting string values
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
  preferences.open("preferences.conf");
  for (auto i: m_int_prefs)
  {
    preferences << i.first << " = " << i.second << "\n";
  }

  for (auto i: m_float_prefs)
  {
    // some funny stuff to ensure the written result has a decimal place because there were problems
    // with writing the value 1.0 which would come out as 1 and read as an integer on the next parse
    preferences << i.first << " = " << std::fixed << std::setprecision(4) << float(i.second) << "\n";
  }

  for (auto i: m_str_prefs)
  {
    preferences << i.first << " = \"" << i.second << "\"\n";
  }
}


const std::map<std::string, int>& Prefs::getIntMap()
{
  return m_int_prefs;
}

const std::map<std::string, float>& Prefs::getFloatMap()
{
  return m_float_prefs;
}

const std::map<std::string, std::string>& Prefs::getStrMap()
{
  return m_str_prefs;
}

PrefType Prefs::getTypeOfPref(const std::string &_key)
{
  for(auto it=m_int_prefs.begin(); it != m_int_prefs.end(); it++)
  {
    if(it->first == _key)
    {
      return PrefType::INT;
    }
  }
  for(auto it=m_float_prefs.begin(); it != m_float_prefs.end(); it++)
  {
    if(it->first == _key)
    {
      return PrefType::FLOAT;
    }
  }
  for(auto it=m_str_prefs.begin(); it != m_str_prefs.end(); it++)
  {
    if(it->first == _key)
    {
      return PrefType::STRING;
    }
  }
  return PrefType::ERROR;
}
