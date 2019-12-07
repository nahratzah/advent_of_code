#include <int_computer.hh>
#include <algorithm>
#include <array>
#include <exception>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

using phase_setting_type = std::array<int, 5>;

struct solution {
  solution() = default;

  constexpr solution(phase_setting_type phase_setting, int_computer_state::value_type output)
  : phase_setting(phase_setting),
    output(output)
  {}

  phase_setting_type phase_setting;
  int_computer_state::value_type output;
};

auto find_solution_(const int_computer_state& program, phase_setting_type& phase_setting, phase_setting_type::size_type idx, int_computer_state::value_type in_value) -> solution {
  if (idx >= phase_setting.size()) throw std::range_error("idx must be in phase setting range");

  std::vector<solution> solutions;
  solutions.reserve(5);
  const bool last = (idx + 1u == phase_setting.size());

  const auto phase_setting_end = phase_setting.begin() + idx;
  for (int s = 0; s < 5; ++s) {
    if (std::find(phase_setting.begin(), phase_setting_end, s) == phase_setting_end) {
      phase_setting[idx] = s;
      const auto eval_out = int_computer_state::single_output(program, { s, in_value });
      if (last) {
        solutions.emplace_back(phase_setting, eval_out);
      } else {
        solutions.push_back(find_solution_(program, phase_setting, idx + 1u, eval_out));
      }
    }
  }

  auto best_iter = std::max_element(solutions.begin(), solutions.end(),
      [](const solution& x, const solution& y) -> bool {
        return x.output < y.output;
      });
  if (best_iter == solutions.end()) throw std::runtime_error("no solutions");
  return *best_iter;
}

auto find_solution(const int_computer_state& program) -> solution {
  phase_setting_type scratch;
  return find_solution_(program, scratch, 0, 0);
}

auto load_computer(std::string filename) -> int_computer_state {
  auto program = std::ifstream(filename);
  return int_computer_state::parse(program);
}

int main(int argc, char* argv[]) {
  const auto progname = argc >= 1 ? argv[0] : "run";

  try {
    std::cout << find_solution(load_computer(std::string(progname) + ".txt")).output << std::endl;
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
}
