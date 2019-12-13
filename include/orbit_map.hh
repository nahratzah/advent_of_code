#ifndef ORBIT_MAP_HH
#define ORBIT_MAP_HH

#include <iosfwd>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>
#include <boost/bimap/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost/bimap/unordered_multiset_of.hpp>


class orbit_map {
  public:
  using satelite = std::string;
  using body = std::string;

  private:
  using map_type = boost::bimaps::bimap<
    boost::bimaps::unordered_multiset_of<body>,
    boost::bimaps::unordered_set_of<satelite>
  >;

  public:
  using size_type = map_type::size_type;

  template<typename Iterator, typename Skip> struct grammar;
  using value_type = map_type::value_type;

  orbit_map() = default;

  orbit_map(std::initializer_list<map_type::value_type> il)
  : map_(il.begin(), il.end())
  {}

  static auto parse(std::istream& in) -> orbit_map;

  template<typename CharT, typename Traits>
  friend auto operator<<(std::basic_ostream<CharT, Traits>& out, const orbit_map& m)
  -> std::basic_ostream<CharT, Traits>& {
    bool first = true;
    for (const auto& e : m.map_.left) {
      if (!std::exchange(first, false)) out << "\n";
      out << e.first << ")" << e.second;
    }

    return out;
  }

  auto empty() const noexcept -> bool { return map_.empty(); }
  auto size() const noexcept -> size_type { return map_.size(); }

  auto satelite_map() const -> map_type::right_map {
    return map_.right;
  }

  auto all_bodies() const -> std::unordered_set<std::string>;

  ///\brief Returns the path to \p s.
  ///\param[in] s The satelite to find.
  ///\param[in] include_s If true, \p s will be included in the result path.
  ///\return Path from the root node to \p s. \p s will be omitted if \p include_s is false.
  auto path(const satelite& s, bool include_s) const -> std::vector<body>;

  private:
  map_type map_;
};


#endif /* ORBIT_MAP_HH */
