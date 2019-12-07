#include <amplifier.hh>
#include <int_computer.hh>
#include <array>
#include <exception>
#include <fstream>
#include <iostream>
#include <functional>
#include <objpipe/callback.h>


auto all_permutations(std::function<void(std::array<int, 5>)> cb) {
  std::array<int, 5> settings;

  for (settings[0] = 5; settings[0] < 10; ++settings[0]) {
    for (settings[1] = 5; settings[1] < 10; ++settings[1]) {
      if (settings[1] == settings[0]) continue;

      for (settings[2] = 5; settings[2] < 10; ++settings[2]) {
        if (settings[2] == settings[0]) continue;
        if (settings[2] == settings[1]) continue;

        for (settings[3] = 5; settings[3] < 10; ++settings[3]) {
          if (settings[3] == settings[0]) continue;
          if (settings[3] == settings[1]) continue;
          if (settings[3] == settings[2]) continue;

          for (settings[4] = 5; settings[4] < 10; ++settings[4]) {
            if (settings[4] == settings[0]) continue;
            if (settings[4] == settings[1]) continue;
            if (settings[4] == settings[2]) continue;
            if (settings[4] == settings[3]) continue;

            cb(settings);
          }
        }
      }
    }
  }
}


auto find_solution(const int_computer_state& program) -> int_computer_state::value_type {
  return objpipe::new_callback<std::array<int, 5>>(all_permutations)
      .transform(
          [&program](const std::array<int, 5>& phase_settings) -> amplifier_chain {
            return amplifier_chain(phase_settings.begin(), phase_settings.end(), program);
          })
      .transform(
          [](amplifier_chain c) -> amplifier_chain::value_type {
            amplifier_chain::value_type v = 0;
            do {
              v = c(v);
            } while (!c.is_halt());
            return v;
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
