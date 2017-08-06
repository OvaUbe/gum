cmake_minimum_required(VERSION 2.8)

project(cppgear-test)

message("Building cppgear-test.")

set(CPPGEAR_TEST_EXTERNAL_LIBS ${CPPGEAR_EXTERNAL_LIBS})

find_package(GTest)
include_directories(${GTEST_INCLUDE_DIRS})
set(CPPGEAR_TEST_EXTERNAL_LIBS ${CPPGEAR_TEST_EXTERNAL_LIBS} ${GTEST_BOTH_LIBRARIES})

string_join(CPPGEAR_TEST_EXTERNAL_LIBS_STRING " " ${CPPGEAR_TEST_EXTERNAL_LIBS})
message("Linking with: " ${CPPGEAR_TEST_EXTERNAL_LIBS_STRING})

file(GLOB_RECURSE CPPGEAR_TEST_SOURCES ${CPPGEAR_TEST_SOURCES} ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
file(GLOB_RECURSE CPPGEAR_TEST_SOURCES ${CPPGEAR_TEST_SOURCES} ${CPPGEAR_SOURCES})

add_executable(cppgear-test EXCLUDE_FROM_ALL ${CPPGEAR_TEST_SOURCES})
set_target_properties(cppgear-test PROPERTIES COMPILE_FLAGS ${CPPGEAR_CXX_COMPILEFLAGS})
target_link_libraries(cppgear-test ${CPPGEAR_TEST_EXTERNAL_LIBS})

add_custom_target(check COMMAND ./cppgear-test)
add_dependencies(check cppgear-test)
