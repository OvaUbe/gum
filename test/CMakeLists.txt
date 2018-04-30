cmake_minimum_required(VERSION 2.8)

project(gum-test)

log(info "Building gum-test.")

set(GUM_TEST_EXTERNAL_LIBS ${GUM_EXTERNAL_LIBS})

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
set(GUM_TEST_EXTERNAL_LIBS ${GUM_TEST_EXTERNAL_LIBS} ${GTEST_BOTH_LIBRARIES})

string_join(GUM_TEST_EXTERNAL_LIBS_STRING " " ${GUM_TEST_EXTERNAL_LIBS})
log(info "Linking with:" ${GUM_TEST_EXTERNAL_LIBS_STRING})

file(GLOB_RECURSE GUM_TEST_SOURCES ${GUM_TEST_SOURCES} ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
file(GLOB_RECURSE GUM_TEST_SOURCES ${GUM_TEST_SOURCES} ${GUM_SOURCES})

add_executable(gum-test EXCLUDE_FROM_ALL ${GUM_TEST_SOURCES})
set_target_properties(gum-test PROPERTIES COMPILE_FLAGS ${GUM_CXX_COMPILEFLAGS})
target_link_libraries(gum-test ${GUM_TEST_EXTERNAL_LIBS})

add_custom_target(check COMMAND ./gum-test)
add_dependencies(check gum-test)
