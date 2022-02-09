cmake_minimum_required(VERSION 3.1)
project(glPrimitives)

add_library(glPrimitives)
target_sources(glPrimitives PRIVATE "src/glPrimitives.cpp")
target_include_directories(glPrimitives PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(glPrimitives PUBLIC glm glad)