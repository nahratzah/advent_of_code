#include <int_computer.hh>
#include <exception>
#include <iostream>

int main(int argc, char* argv[]) {
  const auto progname = argc >= 1 ? argv[0] : "run";

  try {
    int_computer_state ic = int_computer_state::parse(std::cin);
    std::cout << ic.eval_and_get() << std::endl;
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl
        << std::endl
        << "Try for example:" << std::endl
        << "echo 1,9,10,3,2,3,11,0,99,30,40,50 | " << progname << std::endl;
    return 1;
  }
}
