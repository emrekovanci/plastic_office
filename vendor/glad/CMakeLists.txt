cmake_minimum_required(VERSION 3.26)

project(glad)

add_library(glad STATIC
  include/glad/glad.h
  include/KHR/khrplatform.h
  src/glad.c
)

target_include_directories(glad SYSTEM PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

find_package(OpenGL REQUIRED)
target_link_libraries(glad PUBLIC OpenGL::GL)
