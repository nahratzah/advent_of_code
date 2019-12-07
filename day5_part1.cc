#include <int_computer.hh>
#include <exception>
#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
  const auto progname = argc >= 1 ? argv[0] : "run";

  try {
    auto program = std::ifstream(std::string(progname) + ".txt");
    int_computer_state ic = int_computer_state::parse(program);
    program.close();

    ic.read_cb = []() -> int_computer_state::value_type {
      int_computer_state::value_type i;
      std::cin >> i;
      return i;
    };
    ic.write_cb = [](int_computer_state::value_type i) {
      std::cout << i << "\n";
    };

    ic.eval();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl
        << std::endl
        << "Try for example:" << std::endl
        << "echo 1,9,10,3,2,3,11,0,99,30,40,50 | " << progname << std::endl;
    return 1;
  }
}
