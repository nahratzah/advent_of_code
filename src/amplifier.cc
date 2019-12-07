#include <amplifier.hh>
#include <stdexcept>
#include <utility>


amplifier::amplifier(int_computer_state program, int phase_setting)
: s_(std::move(program))
{
  auto io = s_.eval_until_io_or_halt();
  if (io != int_computer_state::io_pending::read)
    throw bad_program_error("first input must be phase setting readout");

  s_.read_cb = [phase_setting]() -> int_computer_state::value_type { return phase_setting; };
  s_.eval1();
  s_.read_cb = nullptr;

  eval_until_read_or_halt_();
}

auto amplifier::operator()(value_type v) -> value_type {
  value_type result;
  if (s_.is_halt()) throw std::runtime_error("amplifier has halted");

  s_.read_cb = [v]() -> int_computer_state::value_type { return v; };
  s_.eval1(); // consume the input
  s_.read_cb = nullptr;

  auto io = s_.eval_until_io_or_halt();
  if (io != int_computer_state::io_pending::write)
    throw bad_program_error("expected amplifier write");
  s_.write_cb = [&result](value_type v) { result = v; };
  s_.eval1(); // write the output
  s_.write_cb = nullptr;

  eval_until_read_or_halt_();
  return result;
}

void amplifier::eval_until_read_or_halt_() {
  switch (s_.eval_until_io_or_halt()) {
    default:
      throw bad_program_error("after phase input, amplifier must perform a read operation");
    case int_computer_state::io_pending::read: [[fallthrough]];
    case int_computer_state::io_pending::halt:
      break;
  }
}


amplifier_chain::amplifier_chain(std::initializer_list<int> phase_settings, const int_computer_state& program)
: amplifier_chain(phase_settings.begin(), phase_settings.end(), program)
{}

auto amplifier_chain::operator()(value_type v) -> value_type {
  for (auto& amp : elems_) v = amp(v);
  return v;
}
