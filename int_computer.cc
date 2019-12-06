#include <int_computer.hh>
#include <iostream>

int main() {
  int_computer_state ic = { 1,9,10,3,2,3,11,0,99,30,40,50 };
  std::cout << ic.eval_and_get() << std::endl;
}
