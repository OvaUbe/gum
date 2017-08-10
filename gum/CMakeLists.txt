cmake_minimum_required(VERSION 2.8)

project(gum)

log(info "Building gum.")

string_join(GUM_EXTERNAL_LIBS_STRING " " ${GUM_EXTERNAL_LIBS})
log(info "Linking with:" ${GUM_EXTERNAL_LIBS_STRING})

add_library(gum-static STATIC ${GUM_SOURCES})
set_target_properties(gum-static PROPERTIES
    COMPILE_FLAGS ${GUM_CXX_COMPILEFLAGS}
    OUTPUT_NAME gum CLEAN_DIRECT_OUTPUT 1)
target_link_libraries(gum-static ${GUM_EXTERNAL_LIBS})

add_library(gum-shared SHARED ${GUM_SOURCES})
set_target_properties(gum-shared PROPERTIES
    COMPILE_FLAGS ${GUM_CXX_COMPILEFLAGS}
    OUTPUT_NAME gum CLEAN_DIRECT_OUTPUT 1)
target_link_libraries(gum-shared ${GUM_EXTERNAL_LIBS})

install_tree(include/gum ${GUM_PUBLIC_HEADERS})
install(TARGETS gum-static gum-shared DESTINATION lib)
