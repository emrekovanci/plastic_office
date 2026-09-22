cmake_minimum_required(VERSION 3.26)

project(
  SSS
  VERSION 0.1.0
  DESCRIPTION "SSS"
  LANGUAGES C CXX
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# -------
# options
# -------
option(ENABLE_ASAN "Enable Address Sanitizer" OFF)
option(ENABLE_ASM_OUTPUT "Enable ASM Output" OFF)
option(BUILD_CONSOLE "Build with console window (useful for debugging)" ON)

function(sss_apply_dev_flags target)
  if(ENABLE_ASAN)
    if(MSVC)
      target_compile_options(${target} PRIVATE /fsanitize=address /Zi)
      target_link_options(${target} PRIVATE /INCREMENTAL:NO)
    else()
      target_compile_options(${target} PRIVATE -fsanitize=address -g)
      target_link_options(${target} PRIVATE -fsanitize=address)
    endif()
  endif()

  if(ENABLE_ASM_OUTPUT AND MSVC)
    target_compile_options(${target} PRIVATE /FA)
  endif()
endfunction()

# ------------
# dependencies
# ------------
add_subdirectory(vendor/glad EXCLUDE_FROM_ALL)
add_subdirectory(vendor/SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendor/cglm EXCLUDE_FROM_ALL)
add_subdirectory(vendor/freetype EXCLUDE_FROM_ALL)
add_subdirectory(vendor/Unity EXCLUDE_FROM_ALL)

# ------------
# game library
# ------------
add_library(game_core STATIC
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
  renderer.h renderer.c
)
target_link_libraries(game_core PUBLIC glad SDL3::SDL3 cglm_headers freetype)
target_include_directories(game_core PUBLIC . vendor/stb)
sss_apply_dev_flags(game_core)

# ---------------
# game executable
# ---------------
if(WIN32 AND NOT BUILD_CONSOLE)
  add_executable(app WIN32 main.c)
else()
  add_executable(app main.c)
endif()
target_link_libraries(app PRIVATE game_core)
sss_apply_dev_flags(app)

add_custom_command(
  TARGET app POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:app>/assets
)

# --------------------------------------------------------------
# font tests (standalone sample binary, not part of ctest suite)
# --------------------------------------------------------------
add_executable(test_font tests/renderer/test_font.c)
target_link_libraries(test_font PRIVATE game_core)
set_target_properties(test_font PROPERTIES EXCLUDE_FROM_ALL TRUE)
sss_apply_dev_flags(test_font)

# ---------------
# unit test suite
# ---------------
enable_testing()

add_executable(tests
  tests/test_tilemap.c
  tests/test_input.c
  tests/test_player.c
  tests/main.c
)
target_link_libraries(tests PRIVATE unity game_core)
set_target_properties(tests PROPERTIES EXCLUDE_FROM_ALL TRUE)
sss_apply_dev_flags(tests)

add_custom_command(
  TARGET tests POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/tests/test_assets $<TARGET_FILE_DIR:tests>/test_assets
)

add_test(
  NAME all_tests
  COMMAND tests
  WORKING_DIRECTORY $<TARGET_FILE_DIR:tests>
)
