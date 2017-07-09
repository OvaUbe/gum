cmake_minimum_required(VERSION 2.8)

include(cmake/utils/Install.cmake)
include(cmake/utils/String.cmake)
include(cmake/utils/Misc.cmake)

find_package(Threads REQUIRED)
set(CPPGEAR_EXTERNAL_LIBS ${CPPGEAR_EXTERNAL_LIBS} ${CMAKE_THREAD_LIBS_INIT})

set(CPPGEAR_CXX_STANDARD_SWITCH "-std=c++14")
set(CPPGEAR_CXX_COMPILER_DIAGNOSTICS_SWITCH "-Wall -Wextra -Wpedantic")

string_join(CPPGEAR_CXX_COMPILEFLAGS " "
    ${CPPGEAR_CXX_STANDARD_SWITCH}
    ${CPPGEAR_CXX_COMPILER_DIAGNOSTICS_SWITCH}
)

message("C++ compile flags: " ${CPPGEAR_CXX_COMPILEFLAGS})

set(CPPGEAR_ROOT ${CMAKE_SOURCE_DIR}/cppgear/)

set(CPPGEAR_SOURCES
    Core.cpp
)
string_prepend(CPPGEAR_SOURCES ${CPPGEAR_ROOT} ${CPPGEAR_SOURCES})

set(CPPGEAR_PUBLIC_HEADERS
    container/FlatMap.h
    container/Iterator.h
    container/Lru.h
    container/TwoQ.h
    Core.h
    Defer.h
    Exception.h
    ExceptionWrapper.h
    IToken.h
    Maybe.h
    Optional.h
    Range.h
    Raw.h
    SharedPtr.h
    SharedReference.h
    Token.h
    UniquePtr.h
    UniqueReference.h
)

include_directories(${CPPGEAR_ROOT})

add_subdirectory(cppgear)
add_subdirectory(test)

highlight_matching(${CPPGEAR_ROOT}/*.h)
highlight_matching(${CPPGEAR_ROOT}/*.cpp)
