cmake_minimum_required(VERSION 3.5)

project(al5_example VERSION 1.0.0 LANGUAGES C)

if(MSVC AND NOT VCPKG_TOOLCHAIN)
  message(FATAL_ERROR "MSVC is only supported when using vcpkg")
endif()

add_executable(al5_example "allegro.c")

find_package(tmx REQUIRED)
target_link_libraries(al5_example PRIVATE tmx)

if(VCPKG_TOOLCHAIN)
  find_package(allegro5 REQUIRED)
  target_link_libraries(al5_example PRIVATE allegro5)
else()
  find_package(PkgConfig REQUIRED)
  pkg_search_module(ALLEGRO5 REQUIRED allegro-5)
  pkg_search_module(ALLEGRO5_IMAGE REQUIRED allegro_image-5)
  pkg_search_module(ALLEGRO5_PRIM REQUIRED allegro_primitives-5)
  target_include_directories(al5_example PRIVATE ${ALLEGRO5_INCLUDE_DIRS})
  target_link_libraries(al5_example PRIVATE ${ALLEGRO5_IMAGE_LIBRARIES} ${ALLEGRO5_PRIM_LIBRARIES})
endif()
