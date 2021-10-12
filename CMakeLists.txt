cmake_minimum_required(VERSION 3.20)
project(Tamara_3D)

set(CMAKE_CXX_STANDARD 20)

if(WIN32) # Windows
    set(SDL2_DIR C:/SDL2-devel-2.0.16-mingw/SDL2-2.0.16/x86_64-w64-mingw32)
else(APPLE) # M1 Homebrew SDL2
    set(SDL2_DIR /opt/homebrew/Cellar/sdl2/2.0.16)
endif()

set(SDL2_LIB_DIR ${SDL2_DIR}/lib)
include_directories(${SDL2_DIR}/include)
add_definitions(-DSDL_MAIN_HANDLED)

add_executable(
        Tamara_3D
        main.cpp
        engine/scene/Scene.h
        engine/scene/object/Object.h
        engine/scene/camera/Camera.h
        engine/scene/object/primitives3/Polygon.h
        engine/utils/Vector3.h
        engine/utils/Vector4.h
        engine/Engine.h
        engine/utils/Color.h
        engine/utils/EulerAngle.h
        engine/scene/object/ObjectLoader.h
        engine/utils/Matrix4.h
        engine/renderer/Renderer.h
        engine/scene/light/Light.h
        engine/scene/light/AmbientLight.h
        engine/scene/light/DiffuseLight.h
        engine/scene/light/SpecularLight.h
        engine/scene/object/primitives3/Vertex.h
        engine/scene/object/primitives3/Line.h
        engine/scene/object/primitives3/Plane.h
        engine/scene/object/primitives2/Point2D.h
        engine/scene/object/primitives2/Line2D.h
)

if(WIN32) # Windows
    target_link_libraries(${PROJECT_NAME} ${SDL2_LIB_DIR}/libSDL2.dll.a ${SDL2_LIB_DIR}/libSDL2main.a)
elseif(APPLE) # M1 Homebrew SDL2
    target_link_libraries(Tamara_3D ${SDL2_LIB_DIR}/libSDL2-2.0.0.dylib ${SDL2_LIB_DIR}/libSDL2main.a)
endif()
