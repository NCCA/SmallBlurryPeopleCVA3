#ifndef __PREFS_HPP__
#define __PREFS_HPP__

#include <map>
#include <string>
#include "ngl/Singleton.h"
#include <iostream>

enum class PrefType
{
  ERROR,
  BOOL,
  INT,
  FLOAT,
  STRING
};

enum class IncType
{
  NONE,    //
  X1,      //
  X10,     //
  X100,    //
  X1000,   //
  X0_1,    //
  X0_01,   //
  X0_001,  //
  X0_0001, //
  POW_2    //
};

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

  IncType getIncType(std::string _key);

  ///
  /// \brief getFloatIncValue get value for incrementing given preference
  /// \param _key name of preference
  /// \return float value to add to preference
  ///
  float getFloatIncValue(const std::string &_key);
  ///
  /// \brief getFloatDecValue get value for decrementing given preference
  /// \param _key name of preference
  /// \return float value to subtract from preference
  ///
  float getFloatDecValue(const std::string &_key);
  ///
  /// \brief getFloatChangeValue get float value required for changing given preference in given direction
  /// \param _key name of preference
  /// \param _dir positive or negative for up/down
  /// \return value to add/subtract
  ///
  float getFloatChangeValue(const std::string &_key, int _dir);

  ///
  /// \brief getIntIncValue get value for incrementing given preference
  /// \param _key name of preference
  /// \return int value to add to preference
  ///
  int getIntIncValue(const std::string &_key);
  ///
  /// \brief getIntDecValue get value for decrementing given preference
  /// \param _key name of preference
  /// \return int value to subtract to preference
  ///
  int getIntDecValue(const std::string &_key);
  ///
  /// \brief getIntChangeValue get int value required for changing given preference in given direction
  /// \param _key name of preference
  /// \param _dir positive or negative for up/down
  /// \return value to add/subtract
  ///
  int getIntChangeValue(const std::string &_key, int _dir);

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

  void setBoolPref(std::string _key, bool _val);

  ///
  /// \brief getIntPref retrieves an integer preference with the given key
  /// \param _key is the string key associated with the value
  /// \return the value of the preference
  ///
  int getIntPref(std::string _key);

  ///
  /// \brief getFloatPref retrieves a float preference with the given key
  /// \param _key is the string key associated with the value
  /// \return the value of the preference
  ///
  float getFloatPref(std::string _key);

  ///
  /// \brief getStrPref retrieves a string preference with the given key
  /// \param _key is the string key associated with the value
  /// \return the value of the preference
  ///
  std::string getStrPref(std::string _key);

  bool getBoolPref(std::string _key);

  ///
  /// \brief getIntMap retrieves the whole integer preference map
  /// \return the map containing all of the integer preference key-value pairs
  ///
  const std::map<std::string, std::pair<int, IncType>> &getIntMap();

  ///
  /// \brief getFloatMap retrieves the entire float preference map
  /// \return the map containing all of the integer preference key-value pairs
  ///
  const std::map<std::string, std::pair<float, IncType>>& getFloatMap();

  ///
  /// \brief getStrMap retrieves the entire string preference map
  /// \return the map containing all of the integer preference key-value pairs
  ///
  const std::map<std::string, std::pair<std::string, IncType>>& getStrMap();

  ///
  /// \brief getBoolMap retrieves the entire string preference map
  /// \return the map containing all of the boolean preference key-value pairs
  ///
  const std::map<std::string, std::pair<bool, IncType>>& getBoolMap();

  ///
  /// \brief printPrefs prints out all key-value pairs stored in the three maps
  ///
  void printPrefs();

  ///
  /// \brief savePrefs saves the preferences to the preferences.conf text file
  ///
  void savePrefs();

  ///
  /// \brief getTypeOfPref find what type the preference is
  /// \param _key name of preference
  /// \return INT, FLOAT or STRING PrefType enum
  ///
  PrefType getTypeOfPref(const std::string &_key);

  ///
  /// \brief setPref generic template for setting preference
  /// \param _key is the string key
  /// \param _val is the value to be associated with the string key, either int, float or string
  ///
  void setPref(std::string &_key, int _val);
  void setPref(std::string &_key, float _val);
  void setPref(std::string &_key, const std::string &_val);
  void setPref(std::string &_key, bool _val);

  ///
  /// \brief getPrefValueString get text for value of string
  /// \param _key preferences key value
  /// \return string representing value, eg "10" if value is 10
  ///
  std::string getPrefValueString(const std::string &_key);

  ///
  /// \brief getNumPrefs get total number of preference options
  /// \return total number of preference options
  ///
  int getNumPrefs();
  ///
  /// \brief getNumChangeablePrefs get number of preferences that can be changed by the Gui
  /// \return number of prefs that can be changed by the Gui
  ///
  int getNumChangeablePrefs();
  ///
  /// \brief boolToString convert bool to string for Gui
  /// \return string, either "0" or "1"
  ///
  std::string boolToString(bool _b);

private:
  ///
  /// \brief The Prefs class is a singlton and therefor needs a private constructor
  ///
  Prefs() = default;

  ///
  /// \brief m_int_prefs a std::map that maps string keys to integer preferences
  ///
  std::map<std::string, std::pair<int, IncType>> m_int_prefs;

  ///
  /// \brief m_float_prefs a std::map that maps string keys to float preferences
  ///
  std::map<std::string, std::pair<float, IncType>> m_float_prefs;

  ///
  /// \brief m_bool_prefs a std::map that maps string keys to boolean preferences
  ///
  std::map<std::string, std::pair<bool, IncType>> m_bool_prefs;

  ///
  /// \brief m_str_prefs a std::map that maps string keys to string preferences
  ///
  std::map<std::string, std::pair<std::string, IncType>> m_str_prefs;
};

#endif//__PREFS_HPP__
