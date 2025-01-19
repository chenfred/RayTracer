#include "util/debug.hpp"

void debug_print(std::string_view msg){
    std::cout << "[DEBUG] " << msg << std::endl;
}