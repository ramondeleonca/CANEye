#ifndef COLOR_H
#define COLOR_H

#include <Arduino.h>

class Color {
    public:
        float x;
        float y;
        float z;

        Color(float x, float y, float z) {
            this->x = x;
            this->y = y;
            this->z = z;
        }
};

#endif