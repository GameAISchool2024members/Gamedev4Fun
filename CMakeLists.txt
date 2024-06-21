cmake_minimum_required(VERSION 3.28)
project(Jam_game)

set(CMAKE_CXX_STANDARD 20)

INCLUDE(FindPkgConfig)

pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_IMG REQUIRED SDL2_image)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
pkg_check_modules(SDL2_MIX REQUIRED SDL2_mixer)

file(GLOB SOURCES game/*.cpp)

add_executable(${PROJECT_NAME} Main.cpp;${SOURCES})

include_directories(${SDL2_INCLUDE_DIRS}
        ${SDL2_IMG_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
        ${SDL2_MIX_INCLUDE_DIRS})

link_directories (${SDL2_LIBRARY_DIRS}
        ${SDL2_IMG_LIBRARY_DIRS}
        ${SDL2_TTF_LIBRARY_DIRS}
        ${SDL2_MIX_LIBRARY_DIRS})

target_link_libraries (${CMAKE_PROJECT_NAME}
        ${SDL2_LIBRARIES}
        ${SDL2_IMG_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
        ${SDL2_MIX_LIBRARIES})

include_directories(${CMAKE_SOURCE_DIR}/game)

add_compile_options(-Wall)
add_compile_options(-x=c++20)
if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_compile_options(-o3)
elseif (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    include(PrintVariables.txt)
    add_compile_options(-g)
endif()