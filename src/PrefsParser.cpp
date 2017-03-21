
#include <boost/bind.hpp>

#include "PrefsParser.hpp"
#include "Prefs.hpp"







namespace spt = boost::spirit::classic;













void PrefsParser::parseFile(std::string _file_name)
{


  std::ifstream fileIn(_file_name.c_str());
  if (fileIn.is_open()){
    std::string line;
    while (std::getline(fileIn, line)){
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
  //std::cout << "parsing int: " << _begin << std::endl;
  std::string key;
  int val;

  srule rule = m_name_rule[spt::assign_a(key)] >> "=" >> spt::int_p[spt::assign_a(val)];
  parse(_begin, rule, spt::space_p);
  m_prefs->setIntPref(key, val);
}

void PrefsParser::parseFloatPref(const char *_begin)
{
  //std::cout << "parsing float: " << _begin << std::endl;
  std::string key;
  float val;

  srule rule = m_name_rule[spt::assign_a(key)] >> "=" >> spt::strict_real_p[spt::assign_a(val)];
  parse(_begin, rule, spt::space_p);
  m_prefs->setFloatPref(key, val);
}

void PrefsParser::parseStrPref(const char *_begin)
{
  //std::cout << "parsing str: " << _begin << std::endl;
  std::string key;
  std::string val;

  srule rule = m_name_rule[spt::assign_a(key)] >> "=" >> '"' >> m_str_rule[spt::assign_a(val)] >> '"';
  parse(_begin, rule, spt::space_p);
  m_prefs->setStrPref(key, val);
}
