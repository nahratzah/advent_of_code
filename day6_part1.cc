#include <orbit_map.hh>
#include <iostream>
#include <objpipe/of.h>


std::unordered_map<orbit_map::satelite, int> make_level_map(const orbit_map& m) {
  std::unordered_map<orbit_map::satelite, int> counters;
  for (auto s : m.all_bodies()) counters[s] = m.path(s, false).size();
  return counters;
}


int main() {
  try {
    const auto total_orbits = objpipe::of(make_level_map(orbit_map::parse(std::cin)))
        .iterate()
        .select_second()
        .reduce(0, [](int x, int y) -> int { return x + y; });
    std::cout << total_orbits << std::endl;
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
}
