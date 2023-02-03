cmake_minimum_required(VERSION 3.24)
project(Halcyonicus)

set(CMAKE_CXX_STANDARD 23)

add_executable(Halcyonicus main.cpp lib/world/world.cpp lib/world/world.h lib/noise/perlin.h)
