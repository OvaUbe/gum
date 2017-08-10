cmake_minimum_required(VERSION 2.8)

project(gum)

include(cmake/utils/Install.cmake)
include(cmake/utils/Log.cmake)
include(cmake/utils/Misc.cmake)
include(cmake/utils/Platform.cmake)
include(cmake/utils/String.cmake)

log(info "Cmake build type:" ${CMAKE_BUILD_TYPE})

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    set(GUM_RELEASE_BUILD False)
    set(GUM_DEBUG_BUILD True)
    register_definitions(GUM_DEBUG_BUILD)
else()
    set(GUM_RELEASE_BUILD True)
    set(GUM_DEBUG_BUILD False)
    register_definitions(GUM_RELEASE_BUILD)
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(GUM_INCLUDE_DEBUG_INFO True)
else()
    set(GUM_INCLUDE_DEBUG_INFO False)
endif()

if(CMAKE_BUILD_TYPE STREQUAL MinSizeRel)
    set(GUM_OPTIMIZE_FOR_SIZE True)
else()
    set(GUM_OPTIMIZE_FOR_SIZE False)
endif()

detect_clang_compiler(GUM_USES_CLANG_COMPILER)
detect_gcc_compiler(GUM_USES_GCC_COMPILER)

if (${GUM_USES_CLANG_COMPILER})
    register_definitions(GUM_USES_CLANG_COMPILER)
elseif (${GUM_USES_GCC_COMPILER})
    register_definitions(GUM_USES_GCC_COMPILER)
endif()

find_package(Threads REQUIRED)
set(GUM_EXTERNAL_LIBS ${GUM_EXTERNAL_LIBS} ${CMAKE_THREAD_LIBS_INIT})

set(GUM_CXX_STANDARD_SWITCH "-std=c++14")
set(GUM_CXX_COMPILER_DIAGNOSTICS_SWITCH "-Wall -Wextra -Wpedantic")

if(${GUM_OPTIMIZE_FOR_SIZE})
    set(GUM_CXX_OPTIMIZATION_SWITCH "-Os")
else()
    if(${GUM_RELEASE_BUILD})
        set(GUM_CXX_OPTIMIZATION_SWITCH "-O3")
    else()
        set(GUM_CXX_OPTIMIZATION_SWITCH "-O0")
    endif()
endif()

if(${GUM_INCLUDE_DEBUG_INFO})
    set(GUM_CXX_DEBUG_INFO_SWITCH "-g")
endif()

string_join(GUM_CXX_COMPILEFLAGS " "
    ${GUM_CXX_STANDARD_SWITCH}
    ${GUM_CXX_COMPILER_DIAGNOSTICS_SWITCH}
    ${GUM_CXX_OPTIMIZATION_SWITCH}
    ${GUM_CXX_DEBUG_INFO_SWITCH}
)

log(info "C++ compile flags:" ${GUM_CXX_COMPILEFLAGS})

set(GUM_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/gum/)

set(GUM_SOURCES
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
set(GUM_PUBLIC_HEADERS
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

if (${GUM_RELEASE_BUILD})
    register_definitions(
        GUM_CONCURRENCY_USES_LIGHTWEIGHT_OWNER_INFO
    )
endif()

if (${GUM_USES_CLANG_COMPILER} OR ${GUM_USES_GCC_COMPILER})
    set(GUM_SOURCES ${GUM_SOURCES}
        backend/gnu/diagnostics/Backtrace.cpp
        backend/gnu/diagnostics/Demangle.cpp
    )
    register_definitions(
        GUM_USES_GNU_BACKEND
    )
endif()

dump_definitions()

string_prepend(GUM_SOURCES ${GUM_ROOT} ${GUM_SOURCES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(gum)
add_subdirectory(test EXCLUDE_FROM_ALL)

highlight_matching(${GUM_ROOT}/*.h)
highlight_matching(${GUM_ROOT}/*.cpp)
