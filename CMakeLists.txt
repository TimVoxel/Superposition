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
FetchContent_MakeAvailable(glfw)

add_library(glad
    external/glad/src/gl.c)

target_include_directories(
    glad
    PUBLIC
        external/glad/include
)

add_executable(Superposition
    src/main.cpp
    src/shader.cpp)

target_include_directories(
    Superposition
    PRIVATE
        include
)

target_link_libraries(Superposition
    PRIVATE
        glfw
        glad)