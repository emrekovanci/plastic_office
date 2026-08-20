cmake_minimum_required(VERSION 3.26)

project(
  SSS
  VERSION 0.1.0
  DESCRIPTION "SSS"
  LANGUAGES C
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# address sanitizer
option(ENABLE_ASAN "Enable Address Sanitizer" OFF)
if(ENABLE_ASAN)
  if(MSVC)
    add_compile_options(/fsanitize=address /Zi)
    add_link_options(/INCREMENTAL:NO)
  else()
    add_compile_options(-fsanitize=address -g)
    add_link_options(-fsanitize=address)
  endif()
endif()

# dependencies
add_subdirectory(vendor/glad EXCLUDE_FROM_ALL)
add_subdirectory(vendor/SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendor/cglm EXCLUDE_FROM_ALL)
add_subdirectory(vendor/freetype EXCLUDE_FROM_ALL)

# game library
add_library(game_lib STATIC
  vendor/stb/stb_image.c
  vendor/stb/stb_ds.c
  color.h
  constants.h
  vertex.h
  utils.h utils.c
  shader.h shader.c
  texture.h texture.c
  camera.h camera.c
  tilemap.h tilemap.c
  player.h player.c
  input.h input.c
  sprite.h sprite.c
  font.h font.c
  window.h window.c
)
target_link_libraries(game_lib PUBLIC glad SDL3::SDL3 cglm_headers freetype)
target_include_directories(game_lib PUBLIC . vendor/stb)

# game executable
# should be like this for production build: add_executable(app WIN32 main.c)
add_executable(app main.c)
target_link_libraries(app PRIVATE game_lib)

# font tests
add_executable(test_font tests/renderer/test_font.c)
target_link_libraries(test_font PRIVATE game_lib)

# if(MSVC)
#   target_compile_options(app PRIVATE /FA)
# endif()

add_custom_command(
  TARGET app POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:app>/assets
)

# test suite
include(CTest)
add_library(unity STATIC vendor/Unity/src/unity.c)
target_include_directories(unity PUBLIC vendor/Unity/src)

add_executable(tests
  tests/test_tilemap.c
  tests/test_input.c
  tests/test_player.c
  tests/main.c
)
target_link_libraries(tests PRIVATE unity game_lib)
set_target_properties(tests PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_custom_command(
  TARGET tests POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/tests/test_assets $<TARGET_FILE_DIR:tests>/test_assets
)

add_test(
  NAME all_tests
  COMMAND tests
  WORKING_DIRECTORY $<TARGET_FILE_DIR:tests>
)
