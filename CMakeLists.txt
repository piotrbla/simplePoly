cmake_minimum_required(VERSION 3.5)
project(simplePoly)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        file.c
        p01.cc
        scopinfo.cc
        scopinfo.hh
        ast.cc
        lex_scheduling.cc
        serial_codegen.cc
        scop.cc
        utility.cc
        options.cc
        codegen.cc
        debug.cc
        tiling.cc
        normalization.cc)
include_directories(/usr/local/lib/)
link_libraries(isl)
link_libraries(pet)

add_executable(simplePoly ${SOURCE_FILES})