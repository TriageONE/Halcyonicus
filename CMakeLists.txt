cmake_minimum_required(VERSION 3.24)
project(Halcyonicus)

set(CMAKE_CXX_STANDARD 23)

if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
endif()

add_executable(Halcyonicus main.cpp lib/world/world.cpp lib/world/world.h lib/noise/perlin.h lib/world/map.cpp lib/world/map.h lib/hash/md5.cpp lib/hash/md5.h lib/entity/entity.cpp lib/entity/entity.h lib/world/cave.cpp lib/world/location.h lib/world/region.cpp lib/world/region.h lib/world/coordinate.h lib/world/worldcoord.h lib/world/regioncoord.h lib/gzip/compress.hpp lib/gzip/config.hpp lib/gzip/decompress.hpp lib/gzip/utils.hpp lib/gzip/version.hpp lib/types/dynablob.h lib/entity/entityTemplate.h lib/entity/entityCluster.h lib/sqlite/sqlite3.c lib/sqlite/sqlite3.h lib/sqlite/sqlite3ext.h lib/hdb/hdb.h lib/entity/entityCluster.cpp lib/entity/entitylocation.h lib/json/json.h lib/types/cfloat.h lib/net/halNet.cpp lib/net/halNet.h lib/crypto/ecdh.h lib/crypto/cc20.h)

find_package(OpenSSL REQUIRED)
target_link_libraries(Halcyonicus PRIVATE OpenSSL::SSL OpenSSL::Crypto)

find_package(unofficial-sodium CONFIG REQUIRED)
target_link_libraries(Halcyonicus PRIVATE unofficial-sodium::sodium)