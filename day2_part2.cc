#include <int_computer.hh>
#include <iostream>
#include <iomanip>

constexpr int_computer_state::value_type SOUGHT = 19690720; 

int main() {
  try {
    const int_computer_state program = int_computer_state::parse(std::cin);
    for (int_computer_state::value_type verb = 0; verb < 100; ++verb) {
      for (int_computer_state::value_type noun = 0; noun < 100; ++noun) {
        auto testcase = program;
        testcase[1] = noun;
        testcase[2] = verb;
        if (testcase.eval_and_get() == SOUGHT)
          std::cout << std::setw(4) << std::right << (100 * noun + verb) << std::endl;
      }
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
}
