cmake_minimum_required(VERSION 3.20)

project(Superposition
    VERSION 0.1
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
)

FetchContent_MakeAvailable(glfw json)

add_library(glad
    external/glad/src/gl.c)

target_include_directories(
    glad
    PUBLIC
        external/glad/include
)

add_executable(Superposition
    src/main.cpp
    src/Shader.cpp
    src/Renderer.cpp
    src/Window.cpp
    src/ParticleSystem.cpp
    src/ParticleSystemConfig.cpp
    src/Config.cpp
    src/VideoWriter.cpp
    src/VideoWriterConfig.cpp
    src/Scene.cpp
    src/Color.cpp
    src/Compositor.cpp
    src/ParticleSystemLink.cpp
    src/Math.cpp
    src/CommandLineArgs.cpp
    src/Transform.cpp
    src/SceneObject.cpp
    src/animation/Animation.cpp
    src/wavefunction/WaveFunction.cpp
    src/wavefunction/HydrogenWaveFunction.cpp
    src/wavefunction/OrbitalFile.cpp)

add_executable(EigenstateSolver
    tools/EigenstateSolver/main.cpp
    tools/EigenstateSolver/EigenstateFinder.cpp
    src/wavefunction/OrbitalFile.cpp)

add_executable(AnalyticalHydrogen
    tools/AnalyticalHydrogen/main.cpp
    src/wavefunction/OrbitalFile.cpp)

target_include_directories(
    Superposition
    PRIVATE
        include
)

target_include_directories(
    EigenstateSolver
    PRIVATE
        include
        tools/EigenstateSolver
)

target_include_directories(
    AnalyticalHydrogen
    PRIVATE
        include
        tools/AnalyticalSolver
)

target_link_libraries(Superposition
    PRIVATE
        glfw
        glad
        nlohmann_json::nlohmann_json)