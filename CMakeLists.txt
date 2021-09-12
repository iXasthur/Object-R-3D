cmake_minimum_required(VERSION 3.20)
project(Tamara_3D)

set(CMAKE_CXX_STANDARD 14)

set(SDL2_PATH "./lib/SDL2-2.0.16/x86_64-w64-mingw32")
find_package(SDL2 REQUIRED)
include_directories( ${SDL2_INCLUDE_DIR} )

add_executable(Tamara_3D src/main.cpp src/engine/Engine3D.h src/utils/Math3D.h)

target_link_libraries(Tamara_3D ${SDL2_LIBRARY})
