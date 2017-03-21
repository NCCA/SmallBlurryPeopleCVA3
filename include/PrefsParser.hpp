#ifndef __PREFSPARSER_HPP__
#define __PREFSPARSER_HPP__

#include <string>
#include <fstream>

#include <boost/spirit/include/classic.hpp>
#include <boost/bind.hpp>

#include <Prefs.hpp>


namespace spt = boost::spirit::classic;
class PrefsParser
{
  typedef boost::spirit::classic::rule<boost::spirit::classic::phrase_scanner_t> srule;
public:
  PrefsParser() = default;
  void parseFile(std::string _file_name);
private:
  void parsePref(const char *_begin);
  void parseIntPref(const char *_begin);
  void parseFloatPref(const char *_begin);
  void parseStrPref(const char *_begin);

  srule m_name_rule = spt::alpha_p >> *spt::alnum_p >> *("_" >> +spt::alnum_p);
  srule m_str_rule = *(spt::anychar_p - '"');
  srule m_quoted_str_rule = '"' >> m_str_rule >> '"';

  srule m_str_pref_rule = (m_name_rule >> "=" >> m_quoted_str_rule)
                      [bind(&PrefsParser::parseStrPref, boost::ref(*this), _1)];
  srule m_int_pref_rule = (m_name_rule >> "=" >> spt::int_p)
                   [bind(&PrefsParser::parseIntPref, boost::ref(*this), _1)];
  srule m_real_pref_rule = (m_name_rule >> "=" >> spt::strict_real_p)
                   [bind(&PrefsParser::parseFloatPref, boost::ref(*this), _1)];
  Prefs* m_prefs = Prefs::instance();
};

#endif//__PREFSPARSER_HPP__
