cmake_minimum_required(VERSION 2.8)

project(cppgear)

message("Building cppgear.")

message("Linking with: " ${CPPGEAR_EXTERNAL_LIBS})

add_library(cppgear-static STATIC ${CPPGEAR_SOURCES})
set_target_properties(cppgear-static PROPERTIES
    COMPILE_FLAGS ${CPPGEAR_CXX_COMPILEFLAGS}
    OUTPUT_NAME cppgear CLEAN_DIRECT_OUTPUT 1)
target_link_libraries(cppgear-static ${CPPGEAR_EXTERNAL_LIBS})

add_library(cppgear-shared SHARED ${CPPGEAR_SOURCES})
set_target_properties(cppgear-shared PROPERTIES
    COMPILE_FLAGS ${CPPGEAR_CXX_COMPILEFLAGS}
    OUTPUT_NAME cppgear CLEAN_DIRECT_OUTPUT 1)
target_link_libraries(cppgear-shared ${CPPGEAR_EXTERNAL_LIBS})

install_tree(include/cppgear ${CPPGEAR_PUBLIC_HEADERS})
install(TARGETS cppgear-static cppgear-shared DESTINATION lib)

file(GLOB_RECURSE CPPGEAR_HEADERS ${CMAKE_CURRENT_LIST_DIR}/*.h)
add_custom_target(_headers SOURCES ${CPPGEAR_HEADERS})
