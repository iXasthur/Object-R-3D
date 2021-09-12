cmake_minimum_required(VERSION 3.20)
project(Tamara_3D)

set(CMAKE_CXX_STANDARD 20)

set(SDL2_DIR C:/SDL2-devel-2.0.16-mingw/SDL2-2.0.16/x86_64-w64-mingw32)
set(SDL2_LIB_DIR ${SDL2_DIR}/lib)

include_directories(${SDL2_DIR}/include)
add_definitions(-DSDL_MAIN_HANDLED)

add_executable(
        Tamara_3D
        main.cpp
        engine/scene/Scene.h
        engine/object/Object.h
        engine/camera/Camera.h
        engine/utils/Polygon.h
        engine/utils/Vector3.h
        engine/utils/Vector4.h
        engine/Engine.h
        engine/utils/Color.h
        engine/utils/EulerAngle.h
        engine/object/ObjectLoader.h
        engine/light/Light.h
        engine/utils/Matrix4.h)

target_link_libraries(${PROJECT_NAME} ${SDL2_LIB_DIR}/libSDL2.dll.a ${SDL2_LIB_DIR}/libSDL2main.a)
