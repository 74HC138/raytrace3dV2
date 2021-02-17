#include "loader.h"

namespace loader {
    Loader::Loader(std::string filename) {
        FILE* fileHandle = fopen(filename.c_str(), "rb");

        std::string fileType = filename.substr(filename.find_last_of(".") + 1);
        char* str = (char*) fileType.c_str();
        for (int i = 0; i < fileType.length(); i++) {
            str[i] = tolower(str[i]);
        }

        if (fileType.compare("stl") == 0) {
            std::cout << "file type is stl\n";
            //file is an stl file
            STL_HEADER stlHeader;
            fread((void*) &stlHeader, sizeof(STL_HEADER), 1, fileHandle);
            
            std::cout << "malloc 1:\n";
            STL_FACET* facets = (STL_FACET*) malloc(sizeof(STL_FACET) * stlHeader.nFacets);
            if (facets == NULL) {
                std::cout << "malloc failed\n";
                std::abort();
            }

            fread((void*) facets, sizeof(STL_FACET), stlHeader.nFacets, fileHandle);
            fclose(fileHandle);

            this->nFacets = stlHeader.nFacets;
            std::cout << "malloc 2:\n";
            this->vertexList = (trace::Vector*) malloc(sizeof(trace::Vector) * stlHeader.nFacets);
            if (this->vertexList == NULL) {
                std::cout << "malloc failed\n";
                std::abort();
            }

            for (int i = 0; i < stlHeader.nFacets; i++) {
                for (int a = 0; a < 3; a++) {
                    this->vertexList[(i * 3) + a] = trace::Vector(facets[i].vertex[a].x, facets[i].vertex[a].y, facets[i].vertex[a].z);
                }
            }

            std::cout << "malloc 3:\n";
            std::cout << sizeof(trace::Object) << "\n";
            this->objectList = (trace::Object*) malloc(sizeof(trace::Object));
            std::cout << "exit malloc\n";
            this->objectList->nTris = stlHeader.nFacets;
            for (int i = 0; i < stlHeader.nFacets; i++) {
                this->objectList->triList[i] = trace::Triangle3D(&this->vertexList[i * 3], trace::Color(1, 1, 1));
            }
            this->objectList->calculateCenter();
            this->objectList->discrimRadius = trace::getMaxDist(this->objectList->centerPoint, this->objectList);

            std::cout << "freeing facets\n";
            free((void*) facets);
            return;
        } else if (fileType.compare("obj") == 0) {
            //file is an obj file
            char* line;
            size_t lineLength;
            int readLength;

            int vertices = 0;
            int textureVertices = 0;
            int normalVertices = 0;
            int objects = 0;

            while ((readLength = getline(&line, &lineLength, fileHandle)) != -1) {
                switch (line[0]) {
                    case 'o':
                        objects++;
                        break;
                    case 'v':
                        switch (line[1]) {
                            case ' ':
                                vertices++;
                                break;
                            case 't':
                                textureVertices++;
                                break;
                            case 'n':
                                normalVertices++;
                                break;
                        }
                        break;
                }
            }
            fseek(fileHandle, 0, SEEK_SET);

            int* nFacets = (int*) malloc(sizeof(int) * objects);
            this->vertexList = (trace::Vector*) malloc(sizeof(trace::Vector) * vertices);
            int currentVertex = 0;
            int currentObject = -1;

            while ((readLength = getline(&line, &lineLength, fileHandle)) != -1) {
                switch (line[0]) {
                    case 'o':
                        currentObject++;
                        break;
                    case 'f':
                        if (currentObject < 0) {
                            std::cout << "facet without atached object ignored\n";
                        } else {
                            nFacets[currentObject]++;
                        }
                        break;
                    case 'v':
                        switch (line[1]) {
                            case ' ':
                                std::string vertexString = &line[2];
                                int firstSpace = vertexString.find_first_of(" ");
                                std::string vertexStringX = vertexString.substr(0, firstSpace);
                                int secondSpace = vertexString.find_first_of(" ", firstSpace + 1);
                                std::string vertexStringY = vertexString.substr(firstSpace + 1, secondSpace);
                                std::string vertexStringZ = vertexString.substr(secondSpace + 1);
                                this->vertexList[currentVertex] = trace::Vector(std::stof(vertexStringX), std::stof(vertexStringY), std::stof(vertexStringZ));
                                currentVertex++;
                        }
                        break;
                }
            }
            fseek(fileHandle, 0, SEEK_SET);

            this->objectList = (trace::Object*) malloc(sizeof(trace::Object) * objects);
            for (int i = 0; i < objects; i++) {
                this->objectList[i].triList = (trace::Triangle3D*) malloc(sizeof(trace::Triangle3D) * nFacets[i]);
                this->objectList[i].nTris = nFacets[i];
            }

            currentObject = -1;
            int currentFacate = 0;

            while ((readLength = getline(&line, &lineLength, fileHandle)) != -1) {
                switch (line[0]) {
                    case 'o':
                        currentObject++;
                        currentFacate = 0;
                        break;
                    case 'f':
                        std::string facetString = &line[2];
                        int firstSpace = facetString.find_first_of(" ");
                        int secondSpace = facetString.find_first_of(" ", firstSpace + 1);
                        std::string vertex1String = facetString.substr(0, firstSpace);
                        std::string vertex2String = facetString.substr(firstSpace + 1, secondSpace);
                        std::string vertex3String = facetString.substr(secondSpace + 1);
                        int vertex1 = stoi(vertex1String);
                        int vertex2 = stoi(vertex2String);
                        int vertex3 = stoi(vertex3String);
                        this->objectList[currentObject].triList[currentFacate] = trace::Triangle3D(this->vertexList[vertex1], this->vertexList[vertex2], this->vertexList[vertex3], trace::Color(1, 1, 1));
                        currentFacate++;
                        break;
                }
            }
            fclose(fileHandle);

            for (int i = 0; i < objects; i++) {
                this->objectList[i].calculateCenter();
                this->objectList[i].discrimRadius = trace::getMaxDist(this->objectList[i].centerPoint, &this->objectList[i]);
            }

            this->nObjects = objects;
        } else {
            std::cout << fileType << " is not a supportet file format\n";
            fclose(fileHandle);
            std::abort();
        }
    }
}