#ifndef __PREFSPARSER_HPP__
#define __PREFSPARSER_HPP__

#include <string>
#include <fstream>
#include <boost/spirit/include/classic.hpp>
#include <boost/bind.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/adapted/std_pair.hpp>
#include "Prefs.hpp"

namespace spt = boost::spirit::classic;
namespace qi = boost::spirit::qi;

template <typename Iterator>
struct int_pair
  : qi::grammar<Iterator, std::map<std::string, int>()>
{
    int_pair()
      : int_pair::base_type(query)
    {
        query =  pair;
        pair  =  key >> qi::lit('=') >> qi::int_;
        key   =  qi::char_("a-zA-Z_") >> *qi::char_("a-zA-Z_0-9");
    }
    qi::rule<Iterator, std::map<std::string, int>()> query;
    qi::rule<Iterator, std::pair<std::string, int>()> pair;
    qi::rule<Iterator, std::string()> key;
};

template <typename Iterator>
struct double_pair
  : qi::grammar<Iterator, std::map<std::string, double>()>
{
    double_pair()
      : double_pair::base_type(query)
    {
        query =  pair;
        pair  =  key >> qi::lit('=') >> strict_double;
        key   =  qi::char_("a-zA-Z_") >> *qi::char_("a-zA-Z_0-9");

    }
    qi::rule<Iterator, std::map<std::string, double>()> query;
    qi::rule<Iterator, std::pair<std::string, double>()> pair;
    qi::rule<Iterator, std::string()> key;
    qi::real_parser<double, qi::strict_real_policies<double>> strict_double;
};

template <typename Iterator>
struct bool_pair
  : qi::grammar<Iterator, std::map<std::string, bool>()>
{
    bool_pair()
      : bool_pair::base_type(query)
    {
        query =  pair;
        pair  =  key >> qi::lit('=') >> qi::bool_;
        key   =  qi::char_("a-zA-Z_") >> *qi::char_("a-zA-Z_0-9");

    }
    qi::rule<Iterator, std::map<std::string, bool>()> query;
    qi::rule<Iterator, std::pair<std::string, bool>()> pair;
    qi::rule<Iterator, std::string()> key;
};

template <typename Iterator>
struct str_pair
  : qi::grammar<Iterator, std::map<std::string, std::string>()>
{
    str_pair()
      : str_pair::base_type(query)
    {
        query =  pair;
        pair  =  key >> qi::lit("=") >> +(qi::char_);
        key   =  qi::char_("a-zA-Z_") >> *qi::char_("a-zA-Z_0-9");

    }
    qi::rule<Iterator, std::map<std::string, std::string>()> query;
    qi::rule<Iterator, std::pair<std::string, std::string>()> pair;
    qi::rule<Iterator, std::string()> key;
};

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

  Prefs* m_prefs = Prefs::instance();
};

#endif//__PREFSPARSER_HPP__
