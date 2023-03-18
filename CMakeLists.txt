cmake_minimum_required(VERSION 3.24)
project(Halcyonicus)

set(CMAKE_CXX_STANDARD 23)

if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
endif()

add_executable(Halcyonicus main.cpp lib/world/world.cpp lib/world/world.h lib/noise/perlin.h lib/world/map.cpp lib/world/map.h lib/hash/md5.cpp lib/hash/md5.h lib/entity/entity.cpp lib/entity/entity.h lib/world/cave.cpp lib/world/location.h lib/world/region.cpp lib/world/region.h lib/world/coordinate.h lib/world/worldcoord.h lib/world/regioncoord.h lib/gzip/compress.hpp lib/gzip/config.hpp lib/gzip/decompress.hpp lib/gzip/utils.hpp lib/gzip/version.hpp lib/entity/dynablob.h)
