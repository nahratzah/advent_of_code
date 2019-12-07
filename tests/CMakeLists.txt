include(CTest)

macro (do_test binary)
  add_executable (test_${binary} ${binary}.cc)
  target_link_libraries (test_${binary} int_computer)
  target_link_libraries (test_${binary} UnitTest++)
  add_test (${binary} test_${binary})
endmacro (do_test)

# Tests here.
do_test(int_computer)
