file(GLOB_RECURSE source CONFIGURE_DEPENDS src/*.cpp)
add_executable(${CMAKE_PROJECT_NAME} ${source})
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE include) #${CMAKE_PROJECT_NAME}是最终可执行文件target，应该没必要被其他target链接

# target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE GLM_FORCE_DEPTH_ZERO_TO_ONE GLM_FORCE_RADIANS GLM_FORCE_LEFT_HANDED) # 调整GLM的坐标系 

target_link_libraries(${CMAKE_PROJECT_NAME} glm stb rapidobj)


