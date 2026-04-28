cmake_minimum_required(VERSION 3.26)

project(
    SSS
    VERSION 0.1.0
    DESCRIPTION "SSS"
    LANGUAGES C
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

set(SDL_EXAMPLES OFF CACHE BOOL "Disable SDL examples" FORCE)
set(SDL_TESTS OFF CACHE BOOL "Disable SDL tests" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "Disable SDL test library" FORCE)
add_subdirectory(vendor/SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendor/glad EXCLUDE_FROM_ALL)
add_subdirectory(vendor/cglm EXCLUDE_FROM_ALL)

add_executable(app
  vendor/stb/stb_image.c
  constants.h
  vertex.h
  direction.h direction.c
  utils.h utils.c
  shader.h shader.c
  texture.h texture.c
  camera.h camera.c
  tilemap.h tilemap.c
  player.h player.c
  input.h input.c
  sprite.h sprite.c
  main.c
)
target_link_libraries(app PRIVATE glad SDL3::SDL3 cglm_headers)
target_include_directories(app PRIVATE vendor/stb)

add_custom_command(
  TARGET app POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:app>/assets
)

# if(MSVC)
#     target_compile_options(app PRIVATE /fsanitize=address)
#     target_link_options(app PRIVATE /fsanitize=address)
# endif()

# if(MSVC)
#   target_compile_options(app PRIVATE /FA)
# endif()
