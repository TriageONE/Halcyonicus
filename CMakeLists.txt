cmake_minimum_required(VERSION 3.25.2)

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
endif()

project(Halcyonicus)

set(CMAKE_CXX_STANDARD 23)

link_directories(./lib ./hsrv ./graphics)

add_executable(Halcyonicus main.cpp halcyonicus.h graphics/initOpengl.cpp)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O2)
endif()


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

set(GRAPHICS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/graphics/shaders)

# Set the destination directory for the assets in the build
set(GRAPHICS_DEST_DIR ${CMAKE_BINARY_DIR}/graphics/shaders)

# Create a custom target to copy the assets
add_custom_target(copy_graphics ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${GRAPHICS_SOURCE_DIR} ${GRAPHICS_DEST_DIR}
        COMMENT "Copying graphics to the build directory"
        SOURCES ${GRAPHICS_SOURCE_DIR}
        )

# Add the custom target as a dependency for your executable or library
add_dependencies(Halcyonicus copy_assets)
add_dependencies(Halcyonicus copy_graphics)

find_package(ZLIB REQUIRED)
target_link_libraries(Halcyonicus ZLIB::ZLIB)

find_package(OpenSSL REQUIRED)
target_link_libraries(Halcyonicus OpenSSL::SSL OpenSSL::Crypto)

find_package(unofficial-sodium CONFIG REQUIRED)
target_link_libraries(Halcyonicus  unofficial-sodium::sodium)

find_package(unofficial-enet CONFIG REQUIRED)
target_link_libraries(Halcyonicus  unofficial::enet::enet)

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(Halcyonicus

        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        )

find_package(SDL2_image CONFIG REQUIRED)
target_link_libraries(Halcyonicus  $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>)

find_package(sdl2-gfx CONFIG REQUIRED)
target_link_libraries(Halcyonicus SDL2::SDL2_gfx)

find_package(GLEW REQUIRED)
target_link_libraries(Halcyonicus GLEW::GLEW)

find_package(glm CONFIG REQUIRED)
target_link_libraries(Halcyonicus glm::glm)

find_package(assimp CONFIG REQUIRED)
target_link_libraries(Halcyonicus assimp::assimp)
