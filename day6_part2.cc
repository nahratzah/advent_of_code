#include <orbit_map.hh>
#include <iostream>
#include <algorithm>
#include <iterator>


std::unordered_map<orbit_map::satelite, int> make_level_map(const orbit_map& m) {
  std::unordered_map<orbit_map::satelite, int> counters;
  for (auto s : m.all_bodies()) counters[s] = m.path(s, false).size();
  return counters;
}

int main() {
  try {
    const auto m = orbit_map::parse(std::cin);
    const auto you_path = m.path("YOU", false); // Designator `YOU` is not a transfer satelite.
    const auto san_path = m.path("SAN", false); // Designator `SAN` is not a transfer satelite.

    auto mismatch_iters = std::mismatch(
        you_path.begin(), you_path.end(),
        san_path.begin(), san_path.end());
    const auto common_len = std::get<0>(mismatch_iters) - you_path.begin();

    std::cout << (you_path.size() + san_path.size() - 2 * common_len) << std::endl;
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
}
