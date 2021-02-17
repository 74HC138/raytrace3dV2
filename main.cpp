#include "trace.h"
#include <iostream>
#include <png++/png.hpp>
#include "parse_stl.h"
#include "loader.h"
#include <malloc.h>

#define WIDTH 500
#define HEIGHT 500
#define FOV 1

png::image <png::rgb_pixel> image(WIDTH, HEIGHT);


int main(int argc, char** argv) {
    std::string fileName;
    if (argc < 2) {
        std::cout << "expected file as parameter!\n";
        return 1;
    }

    //load stl file
    fileName = argv[1];

    loader::Loader load = loader::Loader(fileName);
    int nObjects = load.nObjects;
    std::cout << nObjects << " objects loaded\n";
    std::cout << load.nFacets << " facets loaded\n";
    trace::Object* objectList = load.objectList;

    /*
    auto stlData = stl::parse_stl(fileName);
    if (stlData.triangles.size() <= 0) {
        std::cout << "file not found or corrupt\n";
        return 1;
    } else {
        std::cout << stlData.triangles.size() << " triangles loaded\n";
    }
    std::cout << "stl version:" << stlData.name << "\n";

    trace::Object objectList[1];
    objectList[0].triList = (trace::Triangle3D*) malloc(sizeof(trace::Triangle3D) * stlData.triangles.size());
    if (objectList[0].triList == NULL) {
        std::cout << "malloc failed!\n";
        return 1;
    }
    objectList[0].nTris = stlData.triangles.size();
    objectList[0].centerPoint = trace::Vector(0, 0, 0);
    objectList[0].discrimRadius = 30;

    for (int i = 0; i < stlData.triangles.size(); i++) {
        //std::cout << "triangle: [x:" << stlData.triangles[i].v1.x << " y:" << stlData.triangles[i].v1.y << " z:" << stlData.triangles[i].v1.z << "] [x:" << stlData.triangles[i].v2.x << " y:" << stlData.triangles[i].v2.y << " z:" << stlData.triangles[i].v2.z << "] [x:" << stlData.triangles[i].v3.x << " y:" << stlData.triangles[i].v3.y << " z:" << stlData.triangles[i].v3.z << "]\n";
        objectList[0].triList[i] = trace::Triangle3D(&stlData.triangles[i]);
        //std::cout << "normal vector: x:" << triangleList[i].n.x << " y:" << triangleList[i].n.y << " z:" << triangleList[i].n.z << "\n";
    }
    std::cout << "stl file loading complete -> rendering...\n";
    */

    trace::Vector startPos = trace::Vector(0, 20, -50);
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            trace::Vector direction = trace::Vector(trace::map(x, 0, WIDTH, -FOV, FOV), trace::map(y, 0, HEIGHT, -FOV, FOV), 1);
            direction.normalize();
            direction = direction.rotateX(0.5);
            trace::Ray r = trace::Ray(startPos, direction, 0);
            
            image[y][x] = r.cast(objectList, nObjects).convertToPixel();
        }
    }

    std::cout << "rendering done -> writing to image file...\n";
    image.write("render.png");
    std::cout << "done!\n";
    return 0;
}