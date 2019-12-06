#include <int_computer.hh>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <exception>
#include <iostream>
#include <stdexcept>

auto read_program(std::istream& in) -> int_computer_state {
  using namespace boost::spirit;

  in.unsetf(std::ios::skipws);
  istream_iterator begin = istream_iterator(in);
  istream_iterator end;

  qi::rule<istream_iterator, std::vector<int_computer_state::value_type>(), ascii::space_type> values =
      qi::int_parser<int_computer_state::value_type>() % ',';
  std::vector<int_computer_state::value_type> v;
  if (!qi::phrase_parse(begin, end, values, ascii::space, v))
    throw std::runtime_error("parse failed");

  return int_computer_state(v.begin(), v.end());
}

int main(int argc, char* argv[]) {
  const auto progname = argc >= 1 ? argv[0] : "run";

  try {
    int_computer_state ic = read_program(std::cin);
    std::cout << ic.eval_and_get() << std::endl;
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl
        << std::endl
        << "Try for example:" << std::endl
        << "echo 1,9,10,3,2,3,11,0,99,30,40,50 | " << progname << std::endl;
    return 1;
  }
}
