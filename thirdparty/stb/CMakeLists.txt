add_library(stb STATIC stb_image.cpp)
target_include_directories(stb PUBLIC ${CMAKE_CURRENT_LIST_DIR}) # 用PUBLIC，link到stb的target才能用源代码