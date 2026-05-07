cmake_minimum_required(VERSION 3.26)

project(glad)

add_library(glad STATIC include/glad/glad.h include/KHR/khrplatform.h src/glad.c)
target_include_directories(glad SYSTEM PUBLIC include/)
target_link_libraries(glad PUBLIC opengl32)
