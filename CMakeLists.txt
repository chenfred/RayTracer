cmake_minimum_required(VERSION 3.10)

project(
    RayTracer
    LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# include($ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
# find_package(fmt REQUIRED)

add_subdirectory(thirdparty/glm)

add_subdirectory(source)