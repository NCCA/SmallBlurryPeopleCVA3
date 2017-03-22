#include "PrefsParser.hpp"
#include "Prefs.hpp"

void PrefsParser::parseFile(std::string _file_name)
{
  //open the preferences file
  std::ifstream fileIn(_file_name.c_str());
  if (fileIn.is_open()){
    std::string line;
    //iterate through line by line
    while (std::getline(fileIn, line)){
      //parse each line individually using the three prefernce rules, splitting on spaces
      spt::parse(line.c_str(),
                 m_str_pref_rule |
                 m_real_pref_rule |
                 m_int_pref_rule,
                 spt::space_p);
    }
  }
}

void PrefsParser::parseIntPref(const char *_begin)
{
  //variables to hold the key and value extracted from the line
  std::string key;
  int val;

  //defines a new rule that will assign the values extracted to the variables created above
  srule rule = m_name_rule[spt::assign_a(key)] >> "=" >> spt::int_p[spt::assign_a(val)];

  //parse the line using the previously defined rule
  parse(_begin, rule, spt::space_p);

  //add the new key-value pair to the preferences
  m_prefs->setIntPref(key, val);
}

void PrefsParser::parseFloatPref(const char *_begin)
{
  //variables to hold the key and value extracted from the line
  std::string key;
  float val;

  //defines a new rule that will assign the values extracted to the variables created above
  srule rule = m_name_rule[spt::assign_a(key)] >> "=" >> spt::strict_real_p[spt::assign_a(val)];

  //parse the line using the previously defined rule
  parse(_begin, rule, spt::space_p);

  //add the new key-value pair to the preferences
  m_prefs->setFloatPref(key, val);
}

void PrefsParser::parseStrPref(const char *_begin)
{
  //variables to hold the key and value extracted from the line
  std::string key;
  std::string val;

  //defines a new rule that will assign the values extracted to the variables created above
  srule rule = m_name_rule[spt::assign_a(key)] >> "=" >> '"' >> m_str_rule[spt::assign_a(val)] >> '"';

  //parse the line using the previously defined rule
  parse(_begin, rule, spt::space_p);

  //add the new key-value pair to the preferences
  m_prefs->setStrPref(key, val);
}
