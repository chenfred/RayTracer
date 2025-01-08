cmake_minimum_required(VERSION 3.10)

project(
    RayTracer
    LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

# include($ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
# find_package(fmt REQUIRED)

# 自动添加 thirdparty/ 下的子目录
file(GLOB THIRD_PARTY_DIRS "thirdparty/*")
foreach(DIR ${THIRD_PARTY_DIRS})
    if(IS_DIRECTORY ${DIR})
        add_subdirectory(${DIR})
    endif()
endforeach()
# 添加源代码目录
add_subdirectory(source)
