#include <iostream>
#include <iomanip>
#include <fstream>

#include "Prefs.hpp"
#include "PrefsParser.hpp"
#include "Utility.hpp"

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
  m_bool_prefs.clear();

  //setting new values
  //setting int values
  m_int_prefs["SHADOW_MAP_RES"] = 4096;
  m_int_prefs["WATER_MAP_RES"] = 1024;
  m_int_prefs["X_RES"] = 1920;
  m_int_prefs["Y_RES"] = 1080;

  //setting float values
  m_float_prefs["CHARACTER_SPEED"] = 0.1;
  m_float_prefs["TIME_SCALE"] = 0.01;

  //setting string values
  m_str_prefs["MAP_SCRIPT_PATH"] = "python/simplexMap.py";

  //setting bool values
  m_bool_prefs["AA"] = true;
  m_bool_prefs["DOP"] = true;
  m_bool_prefs["REFLECTIONS"] = true;
  m_bool_prefs["SHADOWS"] = true;
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

void Prefs::setBoolPref(std::string _key, bool _val)
{
  m_bool_prefs[_key] = _val;
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

bool Prefs::getBoolPref(std::string _key)
{
  return m_bool_prefs[_key];
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

  for (auto i: m_bool_prefs)
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

  for (auto i: m_bool_prefs)
  {
    preferences << i.first << " = " << (i.second? "True" : "False") << "\n";
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

const std::map<std::string, bool>& Prefs::getBoolMap()
{
  return m_bool_prefs;
}

PrefType Prefs::getTypeOfPref(const std::string &_key)
{
  for(auto &p : m_int_prefs)
  {
    if(p.first == _key)
    {
      return PrefType::INT;
    }
  }
  for(auto &p : m_float_prefs)
  {
    if(p.first == _key)
    {
      return PrefType::FLOAT;
    }
  }
  for(auto &p : m_bool_prefs)
  {
    if(p.first == _key)
    {
      return PrefType::BOOL;
    }
  }
  for(auto &p : m_str_prefs)
  {
    if(p.first == _key)
    {
      return PrefType::STRING;
    }
  }
  return PrefType::ERROR;
}

void Prefs::setPref(std::string &_key, int _val)
{
  setIntPref(_key, _val);
}

void Prefs::setPref(std::string &_key, float _val)
{
  setFloatPref(_key, _val);
}

void Prefs::setPref(std::string &_key, const std::string &_val)
{
  setStrPref(_key, _val);
}

void Prefs::setPref(std::string &_key, bool _val)
{
  setBoolPref(_key, _val);
}

std::string Prefs::getPrefValueString(const std::string &_key)
{
  PrefType type = getTypeOfPref(_key);
  std::string str_out;
  switch (type) {
  case PrefType::BOOL:
    str_out = boolToString(getBoolPref(_key));
    break;
  case PrefType::INT:
    str_out = std::to_string(getIntPref(_key));
    break;
  case PrefType::FLOAT:
    str_out = std::to_string(getFloatPref(_key));
    break;
  case PrefType::STRING:
    str_out = getStrPref(_key);
    break;
  default:
    std::cerr << "error in setPref, type not known" << std::endl;
    str_out =  "ERROR";
    break;
  }
  return str_out;
}

int Prefs::getNumPrefs()
{
  return m_int_prefs.size() + m_float_prefs.size() + m_bool_prefs.size() +  m_str_prefs.size();
}

std::string Prefs::boolToString(bool _b)
{
  return _b ? "1" : "0";
}
