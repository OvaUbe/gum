cmake_minimum_required(VERSION 2.8)

include(cmake/utils/Install.cmake)
include(cmake/utils/Misc.cmake)
include(cmake/utils/Platform.cmake)
include(cmake/utils/String.cmake)

message("Cmake build type: " ${CMAKE_BUILD_TYPE})

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    set(CPPGEAR_RELEASE_BUILD False)
    set(CPPGEAR_DEBUG_BUILD True)
    register_definitions(CPPGEAR_DEBUG_BUILD)
else()
    set(CPPGEAR_RELEASE_BUILD True)
    set(CPPGEAR_DEBUG_BUILD False)
    register_definitions(CPPGEAR_RELEASE_BUILD)
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(CPPGEAR_INCLUDE_DEBUG_INFO True)
else()
    set(CPPGEAR_INCLUDE_DEBUG_INFO False)
endif()

if(CMAKE_BUILD_TYPE STREQUAL MinSizeRel)
    set(CPPGEAR_OPTIMIZE_FOR_SIZE True)
else()
    set(CPPGEAR_OPTIMIZE_FOR_SIZE False)
endif()

detect_clang_compiler(CPPGEAR_USES_CLANG_COMPILER)
detect_gcc_compiler(CPPGEAR_USES_GCC_COMPILER)

if (${CPPGEAR_USES_CLANG_COMPILER})
    register_definitions(CPPGEAR_USES_CLANG_COMPILER)
elseif (${CPPGEAR_USES_GCC_COMPILER})
    register_definitions(CPPGEAR_USES_GCC_COMPILER)
endif()

find_package(Threads REQUIRED)
set(CPPGEAR_EXTERNAL_LIBS ${CPPGEAR_EXTERNAL_LIBS} ${CMAKE_THREAD_LIBS_INIT})

set(CPPGEAR_CXX_STANDARD_SWITCH "-std=c++14")
set(CPPGEAR_CXX_COMPILER_DIAGNOSTICS_SWITCH "-Wall -Wextra -Wpedantic")

if(${CPPGEAR_OPTIMIZE_FOR_SIZE})
    set(CPPGEAR_CXX_OPTIMIZATION_SWITCH "-Os")
else()
    if(${CPPGEAR_RELEASE_BUILD})
        set(CPPGEAR_CXX_OPTIMIZATION_SWITCH "-O3")
    else()
        set(CPPGEAR_CXX_OPTIMIZATION_SWITCH "-O0")
    endif()
endif()

if(${CPPGEAR_INCLUDE_DEBUG_INFO})
    set(CPPGEAR_CXX_DEBUG_INFO_SWITCH "-g")
endif()

string_join(CPPGEAR_CXX_COMPILEFLAGS " "
    ${CPPGEAR_CXX_STANDARD_SWITCH}
    ${CPPGEAR_CXX_COMPILER_DIAGNOSTICS_SWITCH}
    ${CPPGEAR_CXX_OPTIMIZATION_SWITCH}
    ${CPPGEAR_CXX_DEBUG_INFO_SWITCH}
)

message("C++ compile flags: " ${CPPGEAR_CXX_COMPILEFLAGS})

set(CPPGEAR_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/cppgear/)

set(CPPGEAR_SOURCES
    concurrency/CancellationToken.cpp
    concurrency/DummyCancellationHandle.cpp
    concurrency/LifeToken.cpp
    concurrency/ThreadId.cpp
    concurrency/ThreadInfo.cpp
    concurrency/Thread.cpp
    concurrency/Worker.cpp
    diagnostics/Backtrace.cpp
    diagnostics/Demangle.cpp
    log/sinks/AnsiTerminalLoggerSink.cpp
    log/sinks/StandardLoggerSink.cpp
    log/LogMessage.cpp
    log/LoggerManager.cpp
    log/LoggerStream.cpp
    log/Logger.cpp
    string/ToString.cpp
    Core.cpp
    Enum.cpp
    Exception.cpp
)
set(CPPGEAR_PUBLIC_HEADERS
    async/AsyncFunction.h
    async/ITaskQueue.h
    async/LifeHandle.h
    async/Signal.h
    compare/OwnerLess.h
    concurrency/CancellableFunction.h
    concurrency/CancellationToken.h
    concurrency/ConditionVariable.h
    concurrency/DummyCancellationHandle.h
    concurrency/DummyMutex.h
    concurrency/GenericMutexLock.h
    concurrency/ICancellationToken.h
    concurrency/ImmutableMutexWrapper.h
    concurrency/LifeToken.h
    concurrency/Mutex.h
    concurrency/RwMutex.h
    concurrency/ThreadId.h
    concurrency/ThreadInfo.h
    concurrency/Thread.h
    concurrency/TimedMutexWrapper.h
    concurrency/Worker.h
    container/FlatMap.h
    container/FlatSet.h
    container/Iterator.h
    container/Lru.h
    container/TwoQ.h
    diagnostics/Backtrace.h
    diagnostics/Demangle.h
    functional/Invoker.h
    functional/Types.h
    log/sinks/AnsiTerminalLoggerSink.h
    log/sinks/StandardLoggerSink.h
    log/GlobalLogger.h
    log/ILoggerSink.h
    log/LogLevel.h
    log/LogMessage.h
    log/LoggerId.h
    log/LoggerManager.h
    log/LoggerSingleton.h
    log/LoggerStream.h
    log/Logger.h
    metaprogramming/MethodDetector.h
    smartpointer/SharedPtr.h
    smartpointer/SharedReference.h
    smartpointer/SmartpointerTraits.h
    smartpointer/UniquePtr.h
    smartpointer/UniqueReference.h
    string/StringLiteral.h
    string/String.h
    string/ToString.h
    time/ElapsedTime.h
    time/Types.h
    token/FunctionToken.h
    token/GuardedTokenPool.h
    token/IGuardedTokenPool.h
    token/ITokenPool.h
    token/ObjectToken.h
    token/TokenPool.h
    token/Token.h
    Core.h
    Defer.h
    Enum.h
    Exception.h
    IBoolean.h
    Maybe.h
    Optional.h
    Range.h
    Raw.h
    Singleton.h
    Try.h
    Types.h
)

if (${CPPGEAR_RELEASE_BUILD})
    register_definitions(
        CPPGEAR_CONCURRENCY_USES_LIGHTWEIGHT_OWNER_INFO
    )
endif()

if (${CPPGEAR_USES_CLANG_COMPILER} OR ${CPPGEAR_USES_GCC_COMPILER})
    set(CPPGEAR_SOURCES ${CPPGEAR_SOURCES}
        backend/gnu/diagnostics/Backtrace.cpp
        backend/gnu/diagnostics/Demangle.cpp
    )
    register_definitions(
        CPPGEAR_USES_GNU_BACKEND
    )
endif()

message("Definitions:")
dump_definitions()

string_prepend(CPPGEAR_SOURCES ${CPPGEAR_ROOT} ${CPPGEAR_SOURCES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(cppgear)
add_subdirectory(test)

highlight_matching(${CPPGEAR_ROOT}/*.h)
highlight_matching(${CPPGEAR_ROOT}/*.cpp)
