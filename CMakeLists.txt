cmake_minimum_required(VERSION 3.25.2)

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
endif()

project(Halcyonicus)

set(CMAKE_CXX_STANDARD 23)
if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
endif()


add_executable(Halcyonicus main.cpp lib/context/hcontext.h)


if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O2)
endif()


link_directories(/lib /ui /hsrv)

# Set the source directory of your assets
set(ASSETS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/lib/assets)

# Set the destination directory for the assets in the build
set(ASSETS_DEST_DIR ${CMAKE_BINARY_DIR}/assets)

# Create a custom target to copy the assets
add_custom_target(copy_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS_SOURCE_DIR} ${ASSETS_DEST_DIR}
        COMMENT "Copying assets to the build directory"
        SOURCES ${ASSETS_SOURCE_DIR}
        )

# Add the custom target as a dependency for your executable or library
add_dependencies(Halcyonicus copy_assets)

find_package(OpenSSL REQUIRED)
target_link_libraries(Halcyonicus PRIVATE OpenSSL::SSL OpenSSL::Crypto)

find_package(unofficial-sodium CONFIG REQUIRED)
target_link_libraries(Halcyonicus PRIVATE unofficial-sodium::sodium)

find_package(unofficial-enet CONFIG REQUIRED)
target_link_libraries(Halcyonicus PRIVATE unofficial::enet::enet)

find_package(ZLIB REQUIRED)
target_link_libraries(Halcyonicus PRIVATE ZLIB::ZLIB)

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(Halcyonicus
        PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        )

find_package(SDL2_image CONFIG REQUIRED)
target_link_libraries(Halcyonicus PRIVATE $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>)

find_package(sdl2-gfx CONFIG REQUIRED)
target_link_libraries(Halcyonicus PRIVATE SDL2::SDL2_gfx)

find_package(GLEW REQUIRED)
target_link_libraries(Halcyonicus PRIVATE GLEW::GLEW)

find_package(glm CONFIG REQUIRED)
target_link_libraries(Halcyonicus PRIVATE glm::glm)
