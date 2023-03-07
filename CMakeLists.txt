cmake_minimum_required(VERSION 3.16)
project(broom C)

set(CMAKE_C_STANDARD 99)

include_directories(include)
include_directories(include/ini)

aux_source_directory(src SRC_ALL)
aux_source_directory(src/module MODULE_ALL)
aux_source_directory(src/lib/ini INI_LIB_ALL)

add_executable(broom broom.c ${SRC_ALL} ${MODULE_ALL} ${INI_LIB_ALL})
target_link_libraries(broom pthread)

add_executable(bcli bcli.c)