#ifndef __PREFS_HPP__
#define __PREFS_HPP__

#include <map>
#include <string>

#include "ngl/Singleton.h"

class Prefs : public ngl::Singleton<Prefs>
{
  friend class Singleton<Prefs>;
public:
  void init(std::string _pref_file = "preferences2.conf");
  void restoreDefaultPrefs();
  void setIntPref(std::string _key, int _val);
  void setFloatPref(std::string _key, float _val);
  void setStrPref(std::string _key, std::string _val);
  int getIntPref(std::string _key);
  float getFloatPref(std::string _key);
  std::string getStrPref(std::string _key);
  void printPrefs();
  void savePrefs();
private:
  Prefs() = default;
  std::map<std::string, int> m_int_prefs;
  std::map<std::string, float> m_float_prefs;
  std::map<std::string, std::string> m_str_prefs;
};

#endif//__PREFS_HPP__
