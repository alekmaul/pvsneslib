cmake_minimum_required(VERSION 3.5)

project(raylib_example VERSION 1.0.0 LANGUAGES C)

add_executable(raylib_example "raylib.c")

find_package(tmx REQUIRED)
find_package(raylib REQUIRED) # works with vcpkg

target_link_libraries(raylib_example PRIVATE tmx raylib)

# The following libs should be transitively imported by raylib...
find_package(Threads REQUIRED)
target_link_libraries(raylib_example PRIVATE Threads::Threads)
if(UNIX)
  find_package(X11 REQUIRED)
  target_link_libraries(raylib_example PRIVATE X11::X11)
endif()
