cmake_minimum_required(VERSION 2.8)

include(cmake/utils/Install.cmake)
include(cmake/utils/Misc.cmake)
include(cmake/utils/Platform.cmake)
include(cmake/utils/String.cmake)

detect_clang_compiler(CPPGEAR_USES_CLANG_COMPILER)
detect_gcc_compiler(CPPGEAR_USES_GCC_COMPILER)

if (${CPPGEAR_USES_CLANG_COMPILER})
    add_definitions(-DCPPGEAR_USES_CLANG_COMPILER)
elseif (${CPPGEAR_USES_GCC_COMPILER})
    add_definitions(-DCPPGEAR_USES_GCC_COMPILER)
endif()

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
    diagnostics/Backtrace.cpp
    diagnostics/Demangle.cpp
    Core.cpp
    Exception.cpp
)
set(CPPGEAR_PUBLIC_HEADERS
    concurrency/ConditionVariable.h
    concurrency/ICancellationToken.h
    concurrency/Mutex.h
    container/FlatMap.h
    container/Iterator.h
    container/Lru.h
    container/TwoQ.h
    diagnostics/Backtrace.h
    diagnostics/Demangle.h
    token/FunctionToken.h
    token/ObjectToken.h
    token/Token.h
    Core.h
    Defer.h
    Exception.h
    ExceptionWrapper.h
    IBoolean.h
    Maybe.h
    Optional.h
    Range.h
    Raw.h
    SharedPtr.h
    SharedReference.h
    Types.h
    UniquePtr.h
    UniqueReference.h
)

if (${CPPGEAR_USES_CLANG_COMPILER} OR ${CPPGEAR_USES_GCC_COMPILER})
    set(CPPGEAR_SOURCES ${CPPGEAR_SOURCES}
        backend/gnu/diagnostics/Backtrace.cpp
        backend/gnu/diagnostics/Demangle.cpp
    )
    add_definitions(
        -DCPPGEAR_USES_GNU_BACKEND
    )
endif()

string_prepend(CPPGEAR_SOURCES ${CPPGEAR_ROOT} ${CPPGEAR_SOURCES})

include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(cppgear)
add_subdirectory(test)

highlight_matching(${CPPGEAR_ROOT}/*.h)
highlight_matching(${CPPGEAR_ROOT}/*.cpp)
