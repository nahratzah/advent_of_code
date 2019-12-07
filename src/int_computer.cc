#include <int_computer.hh>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <istream>


bad_program_error::~bad_program_error() = default;

invalid_opcode_error::~invalid_opcode_error() = default;

io_error::~io_error() = default;


auto int_computer_state::eval_and_get() -> value_type {
  return eval().opcodes_[0];
}

auto int_computer_state::eval() -> int_computer_state& {
  while (!is_halt())
    eval1();
  return *this;
}

auto int_computer_state::eval1() -> int_computer_state& {
  if (empty()) throw bad_program_error("empty program");
  assert(pc_ < opcodes_.size());

  const auto& instr_map = instructions();
  const auto instr_iter = instr_map.find(opcode(opcodes_[pc_]));
  if (instr_iter == instr_map.end())
    throw invalid_opcode_error("bad opcode");
  const auto& instr = instr_iter->second;

  // Figure out the arguments.
  const auto arg_start = opcodes_.begin() + pc_ + 1u;
  const auto arg_end = arg_start + instr.arguments;
  if (arg_end > opcodes_.end())
    throw std::range_error("insufficient arguments");

  instr_iter->second.eval(*this, std::vector<instruction::argument_type>(arg_start, arg_end));
  return *this;
}

auto int_computer_state::instructions()
-> const std::unordered_map<opcode, instruction>& {
  static const std::unordered_map<opcode, instruction> map = {
    { opcode::add,   instruction(3, &int_computer_state::instr_add)   },
    { opcode::mul,   instruction(3, &int_computer_state::instr_mul)   },
    { opcode::read,  instruction(1, &int_computer_state::instr_read)  },
    { opcode::write, instruction(1, &int_computer_state::instr_write) },
    { opcode::halt,  instruction(0, &int_computer_state::instr_halt)  }
  };

  return map;
}

auto int_computer_state::operator==(const int_computer_state& y) const noexcept -> bool {
  return pc_ == y.pc_ && opcodes_ == y.opcodes_;
}

void int_computer_state::instr_add(const std::vector<instruction::argument_type>& args) {
  const auto in0 = args[0];
  const auto in1 = args[1];
  const auto out = args[2];

  opcodes_.at(out) = opcodes_.at(in0) + opcodes_.at(in1);
  pc_ += 4u;
}

void int_computer_state::instr_mul(const std::vector<instruction::argument_type>& args) {
  const auto in0 = args[0];
  const auto in1 = args[1];
  const auto out = args[2];

  opcodes_.at(out) = opcodes_.at(in0) * opcodes_.at(in1);
  pc_ += 4u;
}

void int_computer_state::instr_halt(
    [[maybe_unused]] const std::vector<instruction::argument_type>& args) {
  return;
}

void int_computer_state::instr_read(
    const std::vector<instruction::argument_type>& args) {
  const auto pos = args.at(0);

  if (!read_cb) throw io_error("no input");
  opcodes_.at(pos) = read_cb();
  pc_ += 2u;
}

void int_computer_state::instr_write(
    const std::vector<instruction::argument_type>& args) {
  const auto pos = args.at(0);

  if (!write_cb) throw io_error("no output");
  write_cb(opcodes_.at(pos));
  pc_ += 2u;
}


auto int_computer_state::parse(std::istream& in) -> int_computer_state {
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
