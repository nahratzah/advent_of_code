#ifndef INT_COMPUTER_HH
#define INT_COMPUTER_HH

#include <cassert>
#include <cstddef>
#include <functional>
#include <iosfwd>
#include <stdexcept>
#include <tuple>
#include <unordered_map>
#include <vector>


class bad_program_error
: public std::range_error
{
  public:
  using std::range_error::range_error;

  ~bad_program_error();
};

class invalid_opcode_error
: public std::runtime_error
{
  public:
  using std::runtime_error::runtime_error;

  ~invalid_opcode_error();
};

class io_error
: public std::runtime_error
{
  public:
  using std::runtime_error::runtime_error;

  ~io_error();
};

enum class opcode : int {
  add = 1,
  mul = 2,
  read = 3,
  write = 4,
  jump_if_true = 5,
  jump_if_false = 6,
  less_than = 7,
  equals = 8,
  halt = 99
};

enum class addressing_mode : uint8_t {
  position = 0,
  immediate = 1
};

template<typename CharT, typename Traits>
auto operator<<(std::basic_ostream<CharT, Traits>& out, addressing_mode m) -> std::basic_ostream<CharT, Traits>& {
  switch (m) {
  default:
    out << "addressing_mode[" << std::uint32_t(static_cast<std::underlying_type_t<addressing_mode>>(m)) << "]";
    break;
  case addressing_mode::position:
    out << "position";
    break;
  case addressing_mode::immediate:
    out << "immediate";
    break;
  }
  return out;
}

class int_computer_state;


class instruction {
  public:
  using argument_value = int;
  using argument_type = std::tuple<addressing_mode, argument_value>;
  using eval_fn = std::function<void(int_computer_state&, const std::vector<argument_type>&)>;

  instruction() = default;

  instruction(std::size_t arguments, eval_fn eval)
  : arguments(arguments),
    eval(std::move(eval))
  {}

  std::size_t arguments;
  eval_fn eval;
};


class int_computer_state {
  public:
  using opcode_type = std::underlying_type_t<opcode>;
  using value_type = std::common_type_t<opcode_type, instruction::argument_value>;

  private:
  using vector_type = std::vector<value_type>;

  public:
  using size_type = vector_type::size_type;
  using iterator = vector_type::iterator;
  using const_iterator = vector_type::const_iterator;

  int_computer_state() = default;

  int_computer_state(std::initializer_list<value_type> init, size_type pc = 0)
  : opcodes_(init),
    pc_(pc)
  {}

  template<typename Iter>
  int_computer_state(Iter b, Iter e)
  : opcodes_(b, e)
  {}

  static auto parse(std::istream& in) -> int_computer_state;

  auto size() const noexcept -> size_type { return opcodes_.size(); }
  auto empty() const noexcept -> bool { return opcodes_.empty(); }
  auto begin() -> iterator { return opcodes_.begin(); }
  auto end() -> iterator { return opcodes_.end(); }
  auto begin() const -> const_iterator { return opcodes_.begin(); }
  auto end() const -> const_iterator { return opcodes_.end(); }
  auto cbegin() const -> const_iterator { return opcodes_.cbegin(); }
  auto cend() const -> const_iterator { return opcodes_.cend(); }

  auto operator[](size_type idx) -> value_type& {
    assert(idx < size());
    return opcodes_[idx];
  }

  auto operator[](size_type idx) const -> const value_type& {
    assert(idx < size());
    return opcodes_[idx];
  }

  auto is_halt() const -> bool {
    if (empty()) throw bad_program_error("empty program");
    assert(pc_ < opcodes_.size());
    return opcode(opcodes_[pc_]) == opcode::halt;
  }

  auto eval_and_get() -> value_type;
  auto eval() -> int_computer_state&;
  auto eval1() -> int_computer_state&;

  static auto instructions() -> const std::unordered_map<opcode, instruction>&;

  auto operator==(const int_computer_state& y) const noexcept -> bool;

  template<typename CharT, typename Traits>
  friend auto operator<<(std::basic_ostream<CharT, Traits>& out, const int_computer_state& s) -> std::basic_ostream<CharT, Traits>& {
    bool precede_comma = false;
    if (s.pc_ != 0) {
      out << "pc=" << s.pc_;
      precede_comma = true;
    }

    for (const auto& i : s.opcodes_) {
      if (precede_comma) out << ", ";
      out << i;
      precede_comma = true;
    }
    return out;
  }

  private:
  void instr_add(const std::vector<instruction::argument_type>& args);
  void instr_mul(const std::vector<instruction::argument_type>& args);
  void instr_halt(const std::vector<instruction::argument_type>& args);
  void instr_read(const std::vector<instruction::argument_type>& args);
  void instr_write(const std::vector<instruction::argument_type>& args);
  void instr_jump_if_true(const std::vector<instruction::argument_type>& args);
  void instr_jump_if_false(const std::vector<instruction::argument_type>& args);
  void instr_less_than(const std::vector<instruction::argument_type>& args);
  void instr_equals(const std::vector<instruction::argument_type>& args);

  auto get_(instruction::argument_type iarg) const -> value_type;
  void set_(instruction::argument_type iarg, value_type new_value);

  static constexpr auto as_opcode(value_type v) -> opcode {
    return opcode(v % 100);
  }

  static constexpr auto as_modifiers(value_type v) -> value_type {
    return v / 100;
  }

  static constexpr auto get_modifier(value_type v) -> addressing_mode {
    if (v % 10 != 0 && v % 10 != 1)
      throw invalid_opcode_error("invalid addressing mode");
    return addressing_mode(v % 10);
  }

  static constexpr auto shift_modifier(value_type v) -> value_type {
    return v / 10;
  }

  size_type pc_ = 0u;
  vector_type opcodes_;

  public:
  std::function<value_type()> read_cb;
  std::function<void(value_type)> write_cb;
};


inline auto operator!=(const int_computer_state& x, const int_computer_state& y) noexcept -> bool {
  return !(x == y);
}


#endif /* INT_COMPUTER_HH */
