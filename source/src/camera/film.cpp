#include "camera/film.hpp"

Film::Film(size_t _width, size_t _height): width{_width}, height{_height}{
    pixels.resize(width * height);
}