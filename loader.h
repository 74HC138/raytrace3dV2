#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stdint.h>
#include <malloc.h>
#include "trace.h"

namespace loader {
    class Loader {
        public:
            Loader(std::string filename);
            int getFaceCount();
            trace::Vector* getFace(int faceN);
            trace::Vector* getFaces();
            int nFacets;
            trace::Vector* vertexList;
            int nObjects;
            trace::Object* objectList;
    };
    struct STL_HEADER {
        char header[80];
        uint32_t nFacets;
    } __attribute__((__packed__));
    struct STL_VECTOR {
        float x;
        float y;
        float z;
    } __attribute__((__packed__));
    struct STL_FACET {
        STL_VECTOR normal;
        STL_VECTOR vertex[3];
        uint16_t attribute;
    } __attribute__((__packed__));
}