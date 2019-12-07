#include <amplifier.hh>
#include <int_computer.hh>
#include <algorithm>
#include <array>
#include <exception>
#include <fstream>
#include <functional>
#include <iostream>
#include <objpipe/callback.h>


auto all_permutations(std::function<void(std::array<int, 5>)> cb) {
  std::array<int, 5> settings = { 5, 6, 7, 8, 9 };

  do {
    cb(settings);
  } while (std::next_permutation(settings.begin(), settings.end()));
}


auto find_solution(const int_computer_state& program) -> int_computer_state::value_type {
  return objpipe::new_callback<std::array<int, 5>>(all_permutations)
      .transform(
          [&program](const std::array<int, 5>& phase_settings) -> amplifier_chain {
            return amplifier_chain(phase_settings.begin(), phase_settings.end(), program);
          })
      .transform(
          [](amplifier_chain c) -> amplifier_chain::value_type {
            return c.feedback_eval(0);
          })
      .max()
      .value(); // derefence optional
}

auto load_computer(std::string filename) -> int_computer_state {
  auto program = std::ifstream(filename);
  return int_computer_state::parse(program);
}

int main(int argc, char* argv[]) {
  const auto progname = argc >= 1 ? argv[0] : "run";

  try {
    std::cout << find_solution(load_computer(std::string(progname) + ".txt")) << std::endl;
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
}
