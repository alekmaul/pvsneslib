# This is a minimal CMakeLists.txt to link with libTMX and its dependencies
cmake_minimum_required(VERSION 3.5)

project(dumper VERSION 1.0.0 LANGUAGES C)

add_executable(dumper "dumper.c")

# Uses the INSTALL_PREFIX/lib/cmake/tmx/tmxConfig.cmake file to properly link with libTMX
find_package(tmx REQUIRED)

# libTMX exports its target, all dependencies should be transitively imported
target_link_libraries(dumper tmx)
