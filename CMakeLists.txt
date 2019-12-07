cmake_minimum_required(VERSION 3.3)
project (advent_of_code_2019)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)

enable_testing()

include (CheckCXXCompilerFlag)
check_cxx_compiler_flag ("-std=c++17" STD_CXX17)
check_cxx_compiler_flag ("-std=c++1z" STD_CXX1Z)
if(STD_CXX17)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
elseif(STD_CXX1Z)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
else()
  message(WARNING "Cannot figure out how to enable C++17 on compiler.")
endif()

include(CTest)
find_package(UnitTest++)
find_package(Boost)

macro (do_test binary)
  add_executable (test_${binary} ${binary}.cc)
  target_link_libraries (test_${binary} int_computer)
  target_link_libraries (test_${binary} UnitTest++)
  add_test (${binary} test_${binary})
endmacro (do_test)

add_library(int_computer
    src/int_computer.cc
    )

target_include_directories(int_computer PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
target_include_directories(int_computer PUBLIC
    ${Boost_INCLUDE_DIRS})

add_executable(run
    main.cc
    )
target_link_libraries (run int_computer)
target_link_libraries (run ${Boost_LIBRARIES})

if (UnitTest++_FOUND)
  # Tests here.
endif ()
