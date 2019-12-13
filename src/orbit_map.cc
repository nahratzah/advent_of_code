#include <orbit_map.hh>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <boost/phoenix.hpp>
#include <algorithm>
#include <istream>
#include <iterator>
#include <stdexcept>
#include <tuple>


template <typename Iterator, typename Skip>
struct orbit_map::grammar
: boost::spirit::qi::grammar<Iterator, orbit_map(), Skip>
{
  private:
  using pair_type = std::pair<std::string, std::string>;

  static void append_(orbit_map& m, const pair_type& pair) {
    bool success;
    std::tie(std::ignore, success) = m.map_.left.insert(pair);
    if (!success)
      throw std::runtime_error("parse error: duplcate satelite");
  }

  static void pair_set_first_(pair_type& v, pair_type::first_type key) {
    v.first = std::move(key);
  }

  static void pair_set_second_(pair_type& v, pair_type::second_type val) {
    v.second = std::move(val);
  }

  public:
  grammar()
  : grammar::base_type(data_)
  {
    using boost::phoenix::bind;
    using boost::spirit::_val;
    using boost::spirit::_1;
    using boost::spirit::_2;

    data_ = *pair_[bind(grammar::append_, _val, _1)];
    pair_ = key_[bind(grammar::pair_set_first_, _val, _1)] >> ')' >> val_[bind(grammar::pair_set_second_, _val, _1)];
    key_ = +boost::spirit::qi::char_("a-zA-Z0-9");
    val_ = +boost::spirit::qi::char_("a-zA-Z0-9");
  }

  boost::spirit::qi::rule<Iterator, orbit_map(), Skip> data_;
  boost::spirit::qi::rule<Iterator, pair_type(), Skip> pair_;
  boost::spirit::qi::rule<Iterator, pair_type::first_type()> key_;
  boost::spirit::qi::rule<Iterator, pair_type::second_type()> val_;
};


auto orbit_map::parse(std::istream& in) -> orbit_map {
  using namespace boost::spirit;
  orbit_map result;

  in.unsetf(std::ios::skipws);
  istream_iterator begin = istream_iterator(in);
  istream_iterator end;

  grammar<istream_iterator, ascii::space_type> g;
  if (!qi::phrase_parse(begin, end, g, ascii::space, result))
    throw std::runtime_error("parse failed");

  return result;
}

auto orbit_map::all_bodies() const -> std::unordered_set<std::string> {
  std::unordered_set<std::string> result;

  std::transform(
      map_.left.begin(), map_.left.end(),
      std::inserter(result, result.begin()),
      [](const auto& e) -> const auto& { return e.first; });
  std::transform(
      map_.left.begin(), map_.left.end(),
      std::inserter(result, result.begin()),
      [](const auto& e) -> const auto& { return e.second; });
  return result;
}

auto orbit_map::path(const satelite& s, bool include_s) const -> std::vector<body> {
  const satelite* sought = &s;
  std::vector<body> result;
  if (include_s) result.push_back(s);

  map_type::right_map::const_iterator sought_iter;
  while ((sought_iter = map_.right.find(*sought)) != map_.right.end()) {
    result.push_back(sought_iter->second);
    sought = &sought_iter->second;
  }

  std::reverse(result.begin(), result.end());
  return result;
}
