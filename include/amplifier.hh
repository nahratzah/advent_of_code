#ifndef AMPLIFIER_HH
#define AMPLIFIER_HH

#include <int_computer.hh>
#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <utility>
#include <vector>


class amplifier {
  public:
  using value_type = int_computer_state::value_type;

  amplifier() = default;
  amplifier(int_computer_state program, int phase_setting);

  auto empty() const noexcept -> bool {
    return s_.empty();
  }

  auto is_halt() const -> bool {
    return s_.is_halt();
  }

  explicit operator bool() const noexcept {
    return !empty() && !is_halt();
  }

  auto operator!() const noexcept -> bool {
    return empty() || is_halt();
  }

  auto operator()(value_type v) -> value_type;

  private:
  void eval_until_read_or_halt_();

  int_computer_state s_;
};


class amplifier_chain {
  public:
  using value_type = amplifier::value_type;

  amplifier_chain() = default;

  amplifier_chain(std::initializer_list<int> phase_settings, const int_computer_state& program);

  template<typename Iter>
  amplifier_chain(Iter phase_settings_begin, Iter phase_settings_end, const int_computer_state& program) {
    std::transform(phase_settings_begin, phase_settings_end, std::back_inserter(elems_),
        [&program](const auto& phase_setting) -> amplifier {
          return amplifier(program, phase_setting);
        });
  }

  auto is_halt() const -> bool {
    return std::all_of(elems_.begin(), elems_.end(), [](const amplifier& a) { return a.is_halt(); });
  }

  auto operator()(value_type v) -> value_type;
  auto feedback_eval(value_type v) -> value_type;

  auto operator|=(const amplifier& y) -> amplifier_chain& {
    elems_.emplace_back(y);
    return *this;
  }

  auto operator|=(amplifier&& y) -> amplifier_chain& {
    elems_.emplace_back(std::move(y));
    return *this;
  }

  auto operator|(const amplifier& y) const & -> amplifier_chain {
    auto copy = amplifier_chain(*this);
    copy |= y;
    return copy;
  }

  auto operator|(amplifier&& y) const & -> amplifier_chain {
    auto copy = amplifier_chain(*this);
    copy |= std::move(y);
    return copy;
  }

  auto operator|(const amplifier& y) && -> amplifier_chain&& {
    *this |= y;
    return std::move(*this);
  }

  auto operator|(amplifier&& y) && -> amplifier_chain&& {
    *this |= std::move(y);
    return std::move(*this);
  }

  private:
  std::vector<amplifier> elems_;
};


auto operator|(amplifier&& x, amplifier&& y) -> amplifier_chain {
  return amplifier_chain() | std::move(x) | std::move(y);
}

auto operator|(const amplifier& x, amplifier&& y) -> amplifier_chain {
  return amplifier_chain() | x | std::move(y);
}

auto operator|(amplifier&& x, const amplifier& y) -> amplifier_chain {
  return amplifier_chain() | std::move(x) | y;
}

auto operator|(const amplifier& x, const amplifier& y) -> amplifier_chain {
  return amplifier_chain() | x | y;
}


#endif /* AMPLIFIER_HH */
