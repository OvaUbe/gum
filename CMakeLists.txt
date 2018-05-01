cmake_minimum_required(VERSION 3.2)

project(gum)

include(cmake/castella/Install.cmake)
include(cmake/castella/Log.cmake)
include(cmake/castella/Misc.cmake)
include(cmake/castella/Platform.cmake)
include(cmake/castella/Options.cmake)
include(cmake/castella/String.cmake)

include(CheckCSourceCompiles)

set(CMAKE_CXX_STANDARD 14)

log(info "Cmake build type:" ${CMAKE_BUILD_TYPE})
log(info "C++ standard:" ${CMAKE_CXX_STANDARD})

set_option(GUM_BUILD_TESTS OFF "Build tests")
set_option(GUM_USES_BOOST_ASIO ON "Use boost.asio")

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
detect_posix(GUM_USES_POSIX)

CHECK_C_SOURCE_COMPILES(
    "#include <string.h>
    int main() { char b[256]; strerror_r(0, b, sizeof(b)); return 0; }"
    GUM_HAS_STRERROR_R)
if (${GUM_HAS_STRERROR_R})
    register_definitions(GUM_HAS_STRERROR_R)
endif()

if (${GUM_USES_CLANG_COMPILER})
    register_definitions(GUM_USES_CLANG_COMPILER)
elseif (${GUM_USES_GCC_COMPILER})
    register_definitions(GUM_USES_GCC_COMPILER)
endif()

find_package(Threads REQUIRED)
set(GUM_EXTERNAL_LIBS ${GUM_EXTERNAL_LIBS} ${CMAKE_THREAD_LIBS_INIT})

find_package(Boost COMPONENTS system REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
list(APPEND GUM_EXTERNAL_LIBS ${Boost_LIBRARIES})

set(GUM_CXX_COMPILER_DIAGNOSTICS_SWITCH "-Wall -Wextra -Wpedantic")

if(${GUM_OPTIMIZE_FOR_SIZE})
    set(GUM_CXX_OPTIMIZATION_SWITCH "-Os")
else()
    set(GUM_CXX_OPTIMIZATION_SWITCH "-O3")
endif()

if(${GUM_INCLUDE_DEBUG_INFO})
    set(GUM_CXX_DEBUG_INFO_SWITCH "-g")
endif()

string_join(GUM_CXX_COMPILEFLAGS " "
    ${GUM_CXX_COMPILER_DIAGNOSTICS_SWITCH}
    ${GUM_CXX_OPTIMIZATION_SWITCH}
    ${GUM_CXX_DEBUG_INFO_SWITCH}
)

log(info "C++ compile flags:" ${GUM_CXX_COMPILEFLAGS})

set(GUM_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/gum/)

set(GUM_SOURCES
    async/TaskQueue.cpp
    concurrency/CancellationToken.cpp
    concurrency/DummyCancellationHandle.cpp
    concurrency/LifeToken.cpp
    concurrency/ThreadId.cpp
    concurrency/ThreadInfo.cpp
    concurrency/Thread.cpp
    concurrency/Worker.cpp
    diagnostics/Backtrace.cpp
    diagnostics/Demangle.cpp
    exception/ExceptionDetails.cpp
    exception/Exception.cpp
    io/filesystem/FilesystemService.cpp
    log/sinks/AnsiTerminalLoggerSink.cpp
    log/sinks/StandardLoggerSink.cpp
    log/LogMessage.cpp
    log/LoggerManager.cpp
    log/LoggerStream.cpp
    log/Logger.cpp
    string/ToString.cpp
    sys/ErrnoToString.cpp
    Core.cpp
    Enum.cpp
)
set(GUM_PUBLIC_HEADERS
    async/AsyncFunction.h
    async/ITaskQueue.h
    async/LifeHandle.h
    async/Signal.h
    async/TaskQueue.h
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
    container/ContainerTraits.h
    container/HashMap.h
    container/IContainer.h
    container/IMap.h
    container/IObservableMap.h
    container/Iterator.h
    container/Lru.h
    container/ObservableHashMap.h
    container/ObservableSortedMap.h
    container/ObservableStlMap.h
    container/SortedMap.h
    container/StlMap.h
    container/TwoQ.h
    diagnostics/Backtrace.h
    diagnostics/Demangle.h
    exception/ExceptionDetails.h
    exception/Exception.h
    functional/Invoker.h
    functional/Types.h
    io/filesystem/FileOpenFlags.h
    io/filesystem/FilesystemService.h
    io/filesystem/IFile.h
    io/asio/IAsyncByteStream.h
    io/Eof.h
    io/ByteData.h
    io/ISeekable.h
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
    maybe/Maybe.h
    maybe/MaybeTraits.h
    metaprogramming/MethodDetector.h
    metaprogramming/PassingType.h
    smartpointer/DynamicCaster.h
    smartpointer/SharedPtr.h
    smartpointer/SharedReference.h
    smartpointer/SmartpointerTraits.h
    smartpointer/UniquePtr.h
    smartpointer/UniqueReference.h
    string/IStringRepresentable.h
    string/StringLiteral.h
    string/StringRef.h
    string/String.h
    string/ToString.h
    sys/ErrnoToString.h
    sys/SystemException.h
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
    DynamicCaster.h
    Enum.h
    ErrorOr.h
    IBoolean.h
    IObservableValue.h
    ObservableValue.h
    Optional.h
    Overload.h
    Range.h
    Raw.h
    Singleton.h
    Slice.h
    Try.h
    Types.h
    Unit.h
    Visit.h
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

if (${GUM_USES_BOOST_ASIO})
    set(GUM_SOURCES ${GUM_SOURCES}
        backend/boost/asio/BoostAsyncByteStream.cpp
        backend/boost/asio/BoostIoWorker.cpp
    )
    register_definitions(
        GUM_USES_BOOST_ASIO
        BOOST_ASIO_DISABLE_EPOLL
    )
endif()

if (${GUM_USES_POSIX})
    set(GUM_SOURCES ${GUM_SOURCES}
        backend/posix/filesystem/FileDescriptor.cpp
    )
    register_definitions(
        GUM_USES_POSIX
    )
endif()

dump_definitions()

string_prepend(GUM_SOURCES ${GUM_ROOT} ${GUM_SOURCES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(gum)
if (${GUM_BUILD_TESTS})
    add_subdirectory(test EXCLUDE_FROM_ALL)
endif()

highlight_matching(${GUM_ROOT}/*.h)
highlight_matching(${GUM_ROOT}/*.cpp)
