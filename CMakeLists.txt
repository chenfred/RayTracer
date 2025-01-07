cmake_minimum_required(VERSION 3.10)

project(
    RayTracer
    LANGUAGES CXX
)
set(CXX_STANDARD 20)
# set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_HOME}/scripts/buildsystems/vcpkg.cmake CACHE STRING "" FORCE)
include($ENV{VCPKG_ROOT}\\scripts\\buildsystems\\vcpkg.cmake)

find_package(fmt REQUIRED)

add_subdirectory(thirdparty/glm)
add_subdirectory(source)