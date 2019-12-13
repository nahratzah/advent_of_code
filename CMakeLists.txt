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

find_package(UnitTest++)
find_package(Boost)
add_subdirectory(contrib/objpipe)

add_library(int_computer
    src/amplifier.cc
    src/int_computer.cc
    src/orbit_map.cc
    )

target_include_directories(int_computer PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
target_include_directories(int_computer PUBLIC
    ${Boost_INCLUDE_DIRS})

macro (do_executable day part)
  add_executable (day${day}_part${part} day${day}_part${part}.cc)
  target_link_libraries (day${day}_part${part} PUBLIC int_computer ${BOOST_LIBRARIES})
endmacro (do_executable)

do_executable(2 1)
do_executable(2 2)
do_executable(5 1)
do_executable(6 1)
target_link_libraries (day6_part1 PUBLIC objpipe)
do_executable(6 2)
do_executable(7 1)
do_executable(7 2)
target_link_libraries (day7_part2 PUBLIC objpipe)

if (UnitTest++_FOUND)
  add_subdirectory (tests)
endif ()
