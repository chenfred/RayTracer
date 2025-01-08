cmake_minimum_required(VERSION 3.10)

project(
    RayTracer
    LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 20)

# include($ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
# find_package(fmt REQUIRED)

add_subdirectory(thirdparty/glm)

add_subdirectory(source)