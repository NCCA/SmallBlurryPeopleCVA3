#ifndef __PREFS_HPP__
#define __PREFS_HPP__

#include <map>
#include <string>
#include "ngl/Singleton.h"

///
/// \brief The Prefs class holds all of the preferences stored in the file preferences.conf
/// the file is parsed by the PrefsParser class and the contentse are stored in 3 std::maps
/// that link string keys to their corresponding values. The three maps store int, float and
/// string preferences respectively. The Prefs class also has the capacity to restore default
/// parameters and save the current state of the parameters to the same text file they were
/// read from.
///
/// To add a new preference, it only needs to be added to the preferences.conf text file and
/// a default value defined in the function restoreDefaultPrefs()
///
class Prefs : public ngl::Singleton<Prefs>
{
  friend class Singleton<Prefs>;
public:
  ///
  /// \brief init initialises the Prefs class by running the PrefsParser to build the preferences from the given file
  /// \param _pref_file is the file name of the preferences file
  ///
  void init(std::string _pref_file = "preferences.conf");

  ///
  /// \brief restoreDefaultPrefs clears the three maps and fills them with default values
  ///
  void restoreDefaultPrefs();

  ///
  /// \brief setIntPref adds an integer preference key value pair to the interger preferences map
  /// \param _key is the string key, which is the phrase to the left of the equals sign in the conf file
  /// \param _val is the interger value to be asociated with the string key
  ///
  void setIntPref(std::string _key, int _val);

  ///
  /// \brief setFloatPref adds a float preference key value pair to the float preferences map
  /// \param _key is the string key, which is the phrase to the left of the equals sign in the conf file
  /// \param _val is the float value to be asociated with the string key
  ///
  void setFloatPref(std::string _key, float _val);
  ///
  /// \brief setStrPref adds an integer preference key value pair to the interger preferences map
  /// \param _key is the string key, which is the phrase to the left of the equals sign in the conf file
  /// \param _val is the interger value to be asociated with the string key
  ///
  void setStrPref(std::string _key, std::string _val);
  ///
  /// \brief getIntPref
  /// \param _key
  /// \return
  ///
  int getIntPref(std::string _key);
  ///
  /// \brief getFloatPref
  /// \param _key
  /// \return
  ///
  float getFloatPref(std::string _key);
  ///
  /// \brief getStrPref
  /// \param _key
  /// \return
  ///
  std::string getStrPref(std::string _key);
  ///
  /// \brief getIntMap
  /// \return
  ///
  std::map<std::string, int> getIntMap();
  ///
  /// \brief getFloatMap
  /// \return
  ///
  std::map<std::string, float> getFloatMap();

  ///
  /// \brief getStrMap
  /// \return
  ///
  std::map<std::string, std::string> getStrMap();

  ///
  /// \brief printPrefs
  ///
  void printPrefs();

  ///
  /// \brief savePrefs
  ///
  void savePrefs();
private:
  ///
  ///
  ///
  Prefs() = default;

  ///
  /// \brief m_int_prefs
  ///
  std::map<std::string, int> m_int_prefs;

  ///
  /// \brief m_float_prefs
  ///
  std::map<std::string, float> m_float_prefs;

  ///
  /// \brief m_str_prefs
  ///
  std::map<std::string, std::string> m_str_prefs;
};

#endif//__PREFS_HPP__
