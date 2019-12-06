#ifndef INT_COMPUTER_HH
#define INT_COMPUTER_HH

#include <cassert>
#include <cstddef>
#include <functional>
#include <iosfwd>
#include <stdexcept>
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

enum class opcode : int {
  add = 1,
  mul = 2,
  halt = 99
};

class int_computer_state;


class instruction {
  public:
  using argument_type = int;
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
  using value_type = std::common_type_t<opcode_type, instruction::argument_type>;

  private:
  using vector_type = std::vector<value_type>;

  public:
  using size_type = vector_type::size_type;

  int_computer_state() = default;

  int_computer_state(std::initializer_list<value_type> init)
  : opcodes_(init)
  {}

  template<typename Iter>
  int_computer_state(Iter b, Iter e)
  : opcodes_(b, e)
  {}

  static auto parse(std::istream& in) -> int_computer_state;

  auto size() const noexcept -> size_type { return opcodes_.size(); }
  auto empty() const noexcept -> bool { return opcodes_.empty(); }

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

  private:
  void instr_add(const std::vector<instruction::argument_type>& args);
  void instr_mul(const std::vector<instruction::argument_type>& args);
  void instr_halt(const std::vector<instruction::argument_type>& args);

  size_type pc_ = 0u;
  vector_type opcodes_;
};


inline auto operator!=(const int_computer_state& x, const int_computer_state& y) noexcept -> bool {
  return !(x == y);
}


#endif /* INT_COMPUTER_HH */
