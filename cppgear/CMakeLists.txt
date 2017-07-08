cmake_minimum_required(VERSION 2.8)

project(cppgear)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
include_directories(${CMAKE_CURRENT_LIST_DIR}/..)

set(COMPILEFLAGS "-std=c++14 -Wall -Wextra -Wpedantic")

add_library(cppgear-static STATIC ${SOURCES})
set_target_properties(cppgear-static PROPERTIES
    COMPILE_FLAGS ${COMPILEFLAGS}
    OUTPUT_NAME cppgear CLEAN_DIRECT_OUTPUT 1)
target_link_libraries(cppgear-static ${CPPGEAR_EXTERNAL_LIBS})

add_library(cppgear-shared SHARED ${SOURCES})
set_target_properties(cppgear-shared PROPERTIES
    COMPILE_FLAGS ${COMPILEFLAGS}
    OUTPUT_NAME cppgear CLEAN_DIRECT_OUTPUT 1)
target_link_libraries(cppgear-shared ${CPPGEAR_EXTERNAL_LIBS})

install(DIRECTORY ../cppgear DESTINATION include FILES_MATCHING PATTERN "*.h")
install(TARGETS cppgear-static cppgear-shared DESTINATION lib)

file(GLOB_RECURSE CPPGEAR_HEADERS ${CMAKE_CURRENT_LIST_DIR}/*.h)
add_custom_target(_headers SOURCES ${CPPGEAR_HEADERS})
