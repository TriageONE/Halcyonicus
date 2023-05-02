cmake_minimum_required(VERSION 3.25.2)

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
endif()

project(Halcyonicus)

set(CMAKE_CXX_STANDARD 23)

if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
endif()

#link_directories(lib)

add_executable(Halcyonicus main.cpp)

find_package(OpenSSL REQUIRED)
find_package(unofficial-sodium CONFIG REQUIRED)
find_package(unofficial-enet CONFIG REQUIRED)
find_package(ZLIB REQUIRED)

#I Suppose we dont need boost since we are using a modern version of C++
#Finally, i am free
target_link_libraries(Halcyonicus PRIVATE ZLIB::ZLIB)
target_link_libraries(Halcyonicus PRIVATE unofficial::enet::enet)
target_link_libraries(Halcyonicus PRIVATE OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(Halcyonicus PRIVATE unofficial-sodium::sodium)
