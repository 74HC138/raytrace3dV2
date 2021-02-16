#pragma once

#include <math.h>
#include <iostream>
#include <stdint.h>
#include "parse_stl.h"
#include "malloc.h"
#include "png++/png.hpp"

#define LIMIT_X 500
#define LIMIT_Y 500
#define LIMIT_Z 500

namespace trace {
    class Object;

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
            png::rgb_pixel convertToPixel();
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
            Vector rotateX(float theta); //pitch (up down)
            Vector rotateY(float theta); //yaw (left right)
            Vector rotateZ(float theta); //rotation (clockwise anticlockwise)
    };
    class Ray {
        public:
            Vector origin;
            Vector direction;
            int iteration;
            Ray(Vector sp, Vector h, int i);
            Vector evaluate(float d);
            Color cast(Object* objList, int nObjects);
    };
    class Triangle3D {
        public:
            Vector n, uBeta, uGamma;
            float d, kBeta, kGamma;
            Color color;
            Vector centerPoint;
            float discrimRadius;
            Vector vertices[3];
            Triangle3D(Vector vertices[], Color color);
            Triangle3D(stl::triangle* triangle);
            Vector intersectionTest(Ray* r);
            void recalculate();
    };
    class Object {
        public:
            Vector centerPoint;
            float discrimRadius;
            Triangle3D* triList;
            int nTris;
            Vector calculateCenter();
            void transform(float x, float y, float z);
            void transform(Vector transform);
            void rotate(float x, float y, float z);
            void rotate(Vector rotation);
            Object(Triangle3D* triList, int nTris);
            Object();
    };
    float map(float in, float min, float max, float newMin, float newMax);
    Vector getCenterPoint(Vector* vectors, int nVectors);
    Vector getCenterPoint(Object* obj);
    float getMaxDist(Vector center, Vector* vectorList, int nVectors);
    float getMaxDist(Vector center, Object* obj);
    inline int closestTri;
    float getTriDist(Vector origin, Triangle3D* triList, int nTris, bool* dontCare);   
}