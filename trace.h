#pragma once

#include <math.h>
#include <iostream>
#include <stdint.h>
#include "parse_stl.h"
#include "malloc.h"

namespace trace {
    class Color {
        public:
            Color();
            Color(float r, float g, float b);
            Color(float r, float g, float b, float alpha);
            float r;
            float g;
            float b;
            float alpha;
            float brightness;
            uint8_t getR();
            uint8_t getG();
            uint8_t getB();
    };
    class Vector {
        public:
            Vector(float nx, float ny, float nz);
            Vector();
            void normalize();
            float dot(Vector vecB);
            Vector cross(Vector vecB);
            Vector minus(Vector vecB);
            Vector times(float d);
            Vector times(Vector vecB);
            Vector add(Vector vecB);
            float x;
            float y;
            float z;
            float dist();
    };
    class Ray {
        public:
            Vector origin;
            Vector direction;
            int iteration;
            Ray(Vector sp, Vector h, int i);
            Vector evaluate(float d);
    };
    class Triangle3D {
        public:
            Vector n, uBeta, uGamma;
            float d, kBeta, kGamma;
            Color color;
            Triangle3D(Vector vertices[], Color color);
            Triangle3D(stl::triangle* triangle);
            Vector intersectionTest(Ray r);
    };
    float map(float in, float min, float max, float newMin, float newMax);
}