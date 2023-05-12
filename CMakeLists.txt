cmake_minimum_required(VERSION 3.25.2)

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
endif()

project(Halcyonicus)

set(CMAKE_CXX_STANDARD 23)

if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O2)
endif()


link_directories(/lib /ui)

add_executable(Halcyonicus main.cpp)

set(SODIUM_STATIC 1)

find_package(OpenSSL REQUIRED)
find_package(unofficial-sodium CONFIG REQUIRED)
find_package(unofficial-enet CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2-gfx CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)


target_link_libraries(Halcyonicus PRIVATE ZLIB::ZLIB)
target_link_libraries(Halcyonicus PRIVATE unofficial::enet::enet)
target_link_libraries(Halcyonicus PRIVATE OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(Halcyonicus PRIVATE unofficial-sodium::sodium)
target_link_libraries(Halcyonicus
        PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        )
target_link_libraries(Halcyonicus
        PRIVATE
        $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,
        SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
        )
target_link_libraries(Halcyonicus PRIVATE SDL2::SDL2_gfx)