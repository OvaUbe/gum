cmake_minimum_required(VERSION 2.8)

project(cppgear-test)

find_package(GTest)
include_directories(${GTEST_INCLUDE_DIRS})
set(LIBS ${LIBS} ${GTEST_LIBRARIES})

# Project
file(GLOB_RECURSE SOURCES ${SOURCES} ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
file(GLOB_RECURSE SOURCES ${SOURCES} ${CMAKE_CURRENT_LIST_DIR}/../cppgear/*.cpp)
include_directories(${CMAKE_CURRENT_LIST_DIR}/..)

set(COMPILEFLAGS "-std=c++14 -Wall -Wextra -Wpedantic")

add_executable(cppgear-test EXCLUDE_FROM_ALL ${SOURCES})
set_target_properties(cppgear-test PROPERTIES COMPILE_FLAGS ${COMPILEFLAGS})
target_link_libraries(cppgear-test ${LIBS})

add_custom_target(check COMMAND ./cppgear-test)
add_dependencies(check cppgear-test)
