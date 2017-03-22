#ifndef __PREFSPARSER_HPP__
#define __PREFSPARSER_HPP__

#include <string>
#include <fstream>
#include <boost/spirit/include/classic.hpp>
#include <boost/bind.hpp>
#include "Prefs.hpp"

namespace spt = boost::spirit::classic;

///
/// \brief The PrefsParser class is resposible for reading in a preferences text file and
/// parsing the text into integer, float and string preferences that it stores in the Prefs
/// singelton class.
///
/// The parsing is done using boost:spirit::classic which uses ruled defined in an EBNF format
/// to decide what to do with different combinations of characters.
///
class PrefsParser
{
  ///
  /// \brief srule is a typedef local to the Parser that abbreviates access to the boost::spirit::classic library
  ///
  typedef spt:rule<spt::phrase_scanner_t> srule;

public:
  ///
  /// \brief default constructor
  ///
  PrefsParser() = default;

  ///
  /// \brief parseFile parses te given text file and stores the result in an instance of the Pefs singelton class
  /// \param _file_name is the path to the preferences.conf file
  ///
  void parseFile(std::string _file_name);

private:
  ///
  /// \brief parseIntPref parses a int preference, storing the result in the Prefs singelton class
  /// \param _begin is a pointer to the first char in the line containing an int pref
  ///
  void parseIntPref(const char *_begin);

  ///
  /// \brief parseFloatPref parses a float preference, storing the result in the Prefs singelton class
  /// \param _begin is a pointer to the first char in the line containing a float pref
  ///
  void parseFloatPref(const char *_begin);

  ///
  /// \brief parseStrPref parses a string preference, storing the result in the Prefs singelton class
  /// \param _begin is a pointer to the first char in the line containing a string pref
  ///
  void parseStrPref(const char *_begin);

  ///
  /// \brief m_name_rule is a helper variable that defines the format of a preference name in EBNF
  ///
  srule m_name_rule = spt::alpha_p >> *spt::alnum_p >> *("_" >> +spt::alnum_p);

  ///
  /// \brief m_str_rule is a helper varible that defines a string of characters without quotes in EBNF
  ///
  srule m_str_rule = *(spt::anychar_p - '"');

  ///
  /// \brief m_quoted_str_rule is a helper variable that defines a string in quotes
  ///
  srule m_quoted_str_rule = '"' >> m_str_rule >> '"';

  ///
  /// \brief m_str_pref_rule is a helper variable that define the format of a string preference line
  ///
  srule m_str_pref_rule = (m_name_rule >> "=" >> m_quoted_str_rule)
                      [bind(&PrefsParser::parseStrPref, boost::ref(*this), _1)];

  ///
  /// \brief m_int_pref_rule is a helper variable that define the format of an integer preference line
  ///
  srule m_int_pref_rule = (m_name_rule >> "=" >> spt::int_p)
                   [bind(&PrefsParser::parseIntPref, boost::ref(*this), _1)];

  ///
  /// \brief m_real_pref_rule is a helper variable that define the format of a float preference line
  ///
  srule m_real_pref_rule = (m_name_rule >> "=" >> spt::strict_real_p)
                   [bind(&PrefsParser::parseFloatPref, boost::ref(*this), _1)];

  ///
  /// \brief m_prefs is an instance of the Prefs class that will store the result of the parsing
  ///
  Prefs* m_prefs = Prefs::instance();
};

#endif//__PREFSPARSER_HPP__
