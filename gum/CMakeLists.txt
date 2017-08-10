cmake_minimum_required(VERSION 2.8)

project(gum)

include(../cmake/castella/Options.cmake)

log(info "Building gum.")

set_option(GUM_BUILD_STATIC ON "Build static library")
set_option(GUM_BUILD_SHARED OFF "Build shared library")

string_join(GUM_EXTERNAL_LIBS_STRING " " ${GUM_EXTERNAL_LIBS})
log(info "Linking with:" ${GUM_EXTERNAL_LIBS_STRING})

if(${GUM_BUILD_STATIC})
    add_library(gum-static STATIC ${GUM_SOURCES})
    set_target_properties(gum-static PROPERTIES
        COMPILE_FLAGS ${GUM_CXX_COMPILEFLAGS}
        OUTPUT_NAME gum CLEAN_DIRECT_OUTPUT 1)
    target_link_libraries(gum-static ${GUM_EXTERNAL_LIBS})

    install(TARGETS gum-static DESTINATION lib)
endif()

if(${GUM_BUILD_SHARED})
    add_library(gum-shared SHARED ${GUM_SOURCES})
    set_target_properties(gum-shared PROPERTIES
        COMPILE_FLAGS ${GUM_CXX_COMPILEFLAGS}
        OUTPUT_NAME gum CLEAN_DIRECT_OUTPUT 1)
    target_link_libraries(gum-shared ${GUM_EXTERNAL_LIB})

    install(TARGETS gum-shared DESTINATION lib)
endif()

install_tree(include/gum ${GUM_PUBLIC_HEADERS})
