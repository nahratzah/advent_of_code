#include <int_computer.hh>
#include "UnitTest++/UnitTest++.h"
#include <future>


TEST(parse) {
  auto in = std::istringstream("1,9,10,3,2,3,11,0,99,30,40,50");

  CHECK_EQUAL(
      int_computer_state({ 1, 9, 10, 3, 2, 3, 11, 0, 99, 30, 40, 50 }),
      int_computer_state::parse(in));
}

TEST(parse_accepts_spaces) {
  auto in = std::istringstream("1, 9, 10, 3, 2, 3, 11, 0, 99, 30, 40, 50");

  CHECK_EQUAL(
      int_computer_state({ 1, 9, 10, 3, 2, 3, 11, 0, 99, 30, 40, 50 }),
      int_computer_state::parse(in));
}

TEST(parse_accepts_newlines) {
  auto in = std::istringstream("1,\n9,\n10,\n3,\n2,\n3,\n11,\n0,\n99,\n30,\n40,\n50\n");

  CHECK_EQUAL(
      int_computer_state({ 1, 9, 10, 3, 2, 3, 11, 0, 99, 30, 40, 50 }),
      int_computer_state::parse(in));
}

TEST(eval_result) {
  CHECK_EQUAL(
      99,
      int_computer_state({ 99 }).eval_and_get());
  CHECK_EQUAL(
      3500,
      int_computer_state({ 1, 9, 10, 3, 2, 3, 11, 0, 99, 30, 40, 50 }).eval_and_get());
}

TEST(empty) {
  CHECK(int_computer_state().empty());
  CHECK(!int_computer_state({99}).empty());
}

TEST(is_halt) {
  CHECK(int_computer_state({99}).is_halt());
  CHECK(!int_computer_state({1, 0, 0, 0, 99}).is_halt());
}

TEST(instr_add) {
  CHECK_EQUAL(
      3,
      int_computer_state({ 1, 0, 2, 0, 99 }).eval_and_get());
  CHECK_EQUAL(
      77,
      int_computer_state({ 1, 6, 5, 0, 99, 44, 33 }).eval_and_get());
}

TEST(instr_mul) {
  CHECK_EQUAL(
      4,
      int_computer_state({ 2, 0, 2, 0, 99 }).eval_and_get());
  CHECK_EQUAL(
      1452,
      int_computer_state({ 2, 6, 5, 0, 99, 44, 33 }).eval_and_get());
}

TEST(instr_read) {
  int_computer_state ic = { 3, 0, 99 };
  int called = 0;
  ic.read_cb = [&called]() -> int_computer_state::value_type {
    ++called;
    return 17;
  };

  CHECK_EQUAL(
      17,
      ic.eval_and_get());
  CHECK_EQUAL(1, called); // Callback must be invoked exactly once.
}

TEST(instr_write) {
  int_computer_state ic = { 4, 3, 99, 17 };
  // Set up the write callback.
  // We use a promise as that enforces a single call.
  std::future<int_computer_state::value_type> fut;
  std::promise<int_computer_state::value_type> prom;
  fut = prom.get_future();
  ic.write_cb = [&prom](int_computer_state::value_type v) { prom.set_value(v); };

  ic.eval();
  CHECK_EQUAL(17, fut.get());
}

TEST(instr_jump_if_true) {
  CHECK_EQUAL(
      int_computer_state({ 1105, 0, 1, 99 }, 3),
      int_computer_state({ 1105, 0, 1, 99 }).eval1());

  CHECK_EQUAL(
      int_computer_state({ 1105, 1, 1, 99 }, 1),
      int_computer_state({ 1105, 1, 1, 99 }).eval1());
}

TEST(instr_jump_if_false) {
  CHECK_EQUAL(
      int_computer_state({ 1106, 1, 1, 99 }, 3),
      int_computer_state({ 1106, 1, 1, 99 }).eval1());

  CHECK_EQUAL(
      int_computer_state({ 1106, 0, 1, 99 }, 1),
      int_computer_state({ 1106, 0, 1, 99 }).eval1());
}

TEST(instr_less_than) {
  CHECK_EQUAL(
      int_computer_state({ 1107, 0, 1, 1 }, 4),
      int_computer_state({ 1107, 0, 1, 3 }).eval1());

  CHECK_EQUAL(
      int_computer_state({ 1107, 19, 18, 0 }, 4),
      int_computer_state({ 1107, 19, 18, 3 }).eval1());

  CHECK_EQUAL(
      int_computer_state({ 1107, 23, 23, 0 }, 4),
      int_computer_state({ 1107, 23, 23, 3 }).eval1());
}

TEST(instr_equals) {
  CHECK_EQUAL(
      int_computer_state({ 1108, 0, 1, 0 }, 4),
      int_computer_state({ 1108, 0, 1, 3 }).eval1());

  CHECK_EQUAL(
      int_computer_state({ 1108, 19, 18, 0 }, 4),
      int_computer_state({ 1108, 19, 18, 3 }).eval1());

  CHECK_EQUAL(
      int_computer_state({ 1108, 23, 23, 1 }, 4),
      int_computer_state({ 1108, 23, 23, 3 }).eval1());
}

TEST(day5_part2_example_pos_mode_eq_8) {
  for (int x = 0; x < 100; ++x) {
    CHECK_EQUAL(
        x == 8 ? 1 : 0,
        int_computer_state::single_input_single_output({3,9,8,9,10,9,4,9,99,-1,8}, x));
  }
}

TEST(day5_part2_example_pos_mode_lt_8) {
  for (int x = 0; x < 100; ++x) {
    CHECK_EQUAL(
        x < 8 ? 1 : 0,
        int_computer_state::single_input_single_output({3,9,7,9,10,9,4,9,99,-1,8}, x));
  }
}

TEST(day5_part2_example_imm_mode_eq_8) {
  for (int x = 0; x < 100; ++x) {
    CHECK_EQUAL(
        x == 8 ? 1 : 0,
        int_computer_state::single_input_single_output({3,3,1108,-1,8,3,4,3,99}, x));
  }
}

TEST(day5_part2_example_imm_mode_lt_8) {
  for (int x = 0; x < 100; ++x) {
    CHECK_EQUAL(
        x < 8 ? 1 : 0,
        int_computer_state::single_input_single_output({3,3,1107,-1,8,3,4,3,99}, x));
  }
}

int main() {
  return UnitTest::RunAllTests();
}
