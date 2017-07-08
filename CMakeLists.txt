cmake_minimum_required(VERSION 2.8)

find_package(Threads REQUIRED)
set(CPPGEAR_EXTERNAL_LIBS ${CPPGEAR_EXTERNAL_LIBS} ${CMAKE_THREAD_LIBS_INIT})

add_subdirectory(cppgear)
add_subdirectory(test)
