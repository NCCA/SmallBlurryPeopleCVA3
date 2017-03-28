#include "PrefsParser.hpp"
#include "Prefs.hpp"

#include <utility>
void PrefsParser::parseFile(std::string _file_name)
{
  int_pair<std::string::iterator> pi;    // create instance of parser
  double_pair<std::string::iterator> pd;    // create instance of parser
  bool_pair<std::string::iterator> pb;    // create instance of parser
  str_pair<std::string::iterator> ps;    // create instance of parser
  std::map<std::string, int> mi;
  std::map<std::string, double> md;
  std::map<std::string, bool> mb;
  std::map<std::string, std::string> ms;
  //open the preferences file
  std::ifstream fileIn(_file_name.c_str());
  if (fileIn.is_open()){
    std::string line;
    //iterate through line by line
    while (std::getline(fileIn, line)){
      //parse each line individually using the three prefernce rules, splitting on spaces
      std::string::iterator begin = line.begin();
      std::string::iterator end = line.end();
      //bool result = qi::parse(begin, end, pi, mi);   // returns true if successful
      bool resultb = qi::parse(begin, end, ps, ms);
    }
  }
  for(auto p: mi)
  {
    std::cout << p.first << ", " << p.second << std::endl;
  }
  for(auto p: md)
  {
    std::cout << p.first << ", " << p.second << std::endl;
  }
  for(auto p: mb)
  {
    std::cout << p.first << ", " << p.second << std::endl;
  }
  for(auto p: mb)
  {
    std::cout << p.first << ", " << p.second << std::endl;
  }
}

/*
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

void PrefsParser::parseBoolPref(const char *_begin)
{
  //variables to hold the key and value extracted from the line
  std::string key;
  std::string val;
  srule rule = m_name_rule[spt::assign_a(key)] >> "="  >> m_bool_rule[spt::assign_a(val)];
  parse(_begin, rule, spt::space_p);

  if (val == "True")
  {
    m_prefs->setBoolPref(key, true);
  }
  else if (val == "False")
  {
    m_prefs->setBoolPref(key, false);
  }
}
*/
