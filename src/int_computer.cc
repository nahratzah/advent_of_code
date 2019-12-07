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

  const auto opcode_with_modifiers = opcodes_[pc_];

  const auto& instr_map = instructions();
  const auto instr_iter = instr_map.find(as_opcode(opcode_with_modifiers));
  if (instr_iter == instr_map.end())
    throw invalid_opcode_error("bad opcode");
  const auto& instr = instr_iter->second;

  // Figure out the arguments.
  const auto arg_start = opcodes_.begin() + pc_ + 1u;
  const auto arg_end = arg_start + instr.arguments;
  if (arg_end > opcodes_.end())
    throw std::range_error("insufficient arguments");

  // Load argument vector.
  std::vector<instruction::argument_type> iargs;
  iargs.reserve(instr.arguments);
  std::transform(arg_start, arg_end, std::back_inserter(iargs),
      [](const value_type& v) -> instruction::argument_type {
        return std::make_tuple(addressing_mode::position, v);
      });

  // Apply addressing mode to argument vector.
  auto modifiers = as_modifiers(opcode_with_modifiers);
  for (auto& iarg : iargs) {
    std::get<addressing_mode>(iarg) = get_modifier(modifiers);
    modifiers = shift_modifier(modifiers);
  }
  if (modifiers != 0)
    throw invalid_opcode_error("too many opcode modifiers");

  instr_iter->second.eval(*this, std::move(iargs));
  return *this;
}

auto int_computer_state::instructions()
-> const std::unordered_map<opcode, instruction>& {
  static const std::unordered_map<opcode, instruction> map = {
    { opcode::add,           instruction(3, &int_computer_state::instr_add)           },
    { opcode::mul,           instruction(3, &int_computer_state::instr_mul)           },
    { opcode::read,          instruction(1, &int_computer_state::instr_read)          },
    { opcode::write,         instruction(1, &int_computer_state::instr_write)         },
    { opcode::jump_if_true,  instruction(2, &int_computer_state::instr_jump_if_true)  },
    { opcode::jump_if_false, instruction(2, &int_computer_state::instr_jump_if_false) },
    { opcode::halt,          instruction(0, &int_computer_state::instr_halt)          }
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

  set_(out, get_(in0) + get_(in1));
  pc_ += 4u;
}

void int_computer_state::instr_mul(const std::vector<instruction::argument_type>& args) {
  const auto in0 = args[0];
  const auto in1 = args[1];
  const auto out = args[2];

  set_(out, get_(in0) * get_(in1));
  pc_ += 4u;
}

void int_computer_state::instr_halt(
    [[maybe_unused]] const std::vector<instruction::argument_type>& args) {
  return;
}

void int_computer_state::instr_read(const std::vector<instruction::argument_type>& args) {
  const auto pos = args.at(0);

  if (!read_cb) throw io_error("no input");
  set_(pos, read_cb());
  pc_ += 2u;
}

void int_computer_state::instr_write(const std::vector<instruction::argument_type>& args) {
  const auto pos = args.at(0);

  if (!write_cb) throw io_error("no output");
  write_cb(get_(pos));
  pc_ += 2u;
}

void int_computer_state::instr_jump_if_true(const std::vector<instruction::argument_type>& args) {
  const auto v = args.at(0);
  const auto new_pc = args.at(1);

  if (get_(v) != 0) {
    pc_ = get_(new_pc);
  } else {
    pc_ += 3u;
  }
}

void int_computer_state::instr_jump_if_false(const std::vector<instruction::argument_type>& args) {
  const auto v = args.at(0);
  const auto new_pc = args.at(1);

  if (get_(v) == 0) {
    pc_ = get_(new_pc);
  } else {
    pc_ += 3u;
  }
}

auto int_computer_state::get_(instruction::argument_type iarg) const -> value_type {
  const auto v = std::get<instruction::argument_value>(iarg);

  switch (std::get<addressing_mode>(iarg)) {
    case addressing_mode::position:
      return opcodes_.at(v);
    case addressing_mode::immediate:
      return v;
  }
}

void int_computer_state::set_(instruction::argument_type iarg, value_type new_value) {
  const auto v = std::get<instruction::argument_value>(iarg);

  switch (std::get<addressing_mode>(iarg)) {
    case addressing_mode::position:
      opcodes_.at(v) = new_value;
      break;
    case addressing_mode::immediate:
      throw invalid_opcode_error("cannot assign to a immediate value");
  }
}


auto int_computer_state::parse(std::istream& in) -> int_computer_state {
  using namespace boost::spirit;
  int_computer_state result;

  in.unsetf(std::ios::skipws);
  istream_iterator begin = istream_iterator(in);
  istream_iterator end;

  qi::rule<istream_iterator, std::vector<int_computer_state::value_type>(), ascii::space_type> values =
      qi::int_parser<int_computer_state::value_type>() % ',';
  if (!qi::phrase_parse(begin, end, values, ascii::space, result.opcodes_))
    throw std::runtime_error("parse failed");

  return result;
}
