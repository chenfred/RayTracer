file(GLOB_RECURSE source CONFIGURE_DEPENDS src/*.cpp)

add_executable(${CMAKE_PROJECT_NAME} ${source})

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC include)

target_link_libraries(${CMAKE_PROJECT_NAME} glm)