#include "trace.h"

namespace trace {
    Vector::Vector(float nx, float ny, float nz) {
        this->x = nx;
        this->y = ny;
        this->z = nz;
    }
    Vector::Vector() {
        this->x = 0;
        this->y = 0;
        this->z = 0;
    }
    void Vector::normalize() {
        float d = dist();
        this->x = this->x / d;
        this->y = this->y / d;
        this->z = this->z / d;
    }
    float Vector::dot(Vector vecB) {
        float ret;
        ret = this->x * vecB.x;
        ret += this->y * vecB.y;
        ret += this->z * vecB.z;
        return ret;
    }
    Vector Vector::cross(Vector B) {
        Vector ret = Vector(0, 0, 0);
        ret.x = (this->y * B.z) - (this->z * B.y);
        ret.y = (this->z * B.x) - (this->x * B.z);
        ret.z = (this->x * B.y) - (this->y * B.x);
        return ret; 
    }
    float Vector::dist() {
        float d = sqrt(pow(abs(this->x), 2) + pow(abs(this->y), 2) + pow(abs(this->z), 2));
        return d;
    }
    Vector Vector::minus(Vector vecB) {
        Vector ret = Vector(0, 0, 0);
        ret.x = this->x - vecB.x;
        ret.y = this->y - vecB.y;
        ret.z = this->z - vecB.z;
        return ret;
    }
    Vector Vector::times(float d) {
        Vector ret = Vector(0, 0, 0);
        ret.x = this->x * d;
        ret.y = this->y * d;
        ret.z = this->z * d;
        return ret;
    }
    Vector Vector::add(Vector vecB) {
        Vector ret = Vector(0, 0, 0);
        ret.x = this->x + vecB.x;
        ret.y = this->y + vecB.y;
        ret.z = this->z + vecB.z;
        return ret;
    }
    Vector Vector::times(Vector vecB) {
        Vector ret;
        ret.x = this->x * vecB.x;
        ret.y = this->y * vecB.y;
        ret.z = this->z * vecB.z;
        return ret;
    }
    Vector Vector::rotateX(float theta) {
        Vector ret;
        ret.x = this->x;
        ret.y = (this->y * cos(theta)) - (this->z * sin(theta));
        ret.z = (this->y * sin(theta)) + (this->z * cos(theta));
        return ret;
    }
    Vector Vector::rotateY(float theta) {
        Vector ret;
        ret.x = (this->x * cos(theta)) + (this->z * sin(theta));
        ret.y = this->y;
        ret.z = (-this->x * sin(theta)) + (this->z * cos(theta));
        return ret; 
    }
    Vector Vector::rotateZ(float theta) {
        Vector ret;
        ret.x = (this->x * cos(theta)) - (this->y * sin(theta));
        ret.y = (this->x * sin(theta)) + (this->y * cos(theta));
        ret.z = this->z;
        return ret;
    }
    Ray::Ray(Vector sp, Vector h, int i) {
        this->origin = sp;
        this->direction = h;
        this->iteration = i;
    }
    Vector Ray::evaluate(float d) {
        Vector ret = this->origin;
        ret.x += this->direction.x * d;
        ret.y += this->direction.y * d;
        ret.z += this->direction.z * d;
        return ret;
    }
    Color Ray::cast(Object* objList, int nObjects) {
        if (objList == NULL) return Color(255, 0, 0); //return error color
        while (42) {
            float minDist = this->origin.minus(objList[0].centerPoint).dist() - objList[0].discrimRadius;
            int closestObj = 0;
            for (int i = 0; i < nObjects; i++) {
                float tmp = this->origin.minus(objList[i].centerPoint).dist() - objList[i].discrimRadius;
                if (tmp < minDist) {
                    minDist = tmp;
                    closestObj = i;
                } 
            }
            if (minDist < 0.1) {
                //cast in object
                int closestTri = -1;
                float closestDist = 9999999;
                for (int i = 0; i < objList[closestObj].nTris; i++) {
                    Vector tmp = objList[closestObj].triList[i].intersectionTest(this);
                    if (tmp.minus(this->origin).dist() > 0.01) {
                        if (tmp.minus(this->origin).dist() < closestDist) {
                            closestDist = tmp.minus(this->origin).dist();
                            closestTri = i;
                        }
                    }
                }
                if (closestTri < 0) {
                    this->origin = this->origin.add(this->direction.times(objList[closestObj].discrimRadius));
                    Vector diff = this->origin.minus(objList[closestObj].centerPoint);
                    diff.normalize();
                    diff.times(objList[closestObj].discrimRadius + 1);
                    this->origin = objList[closestObj].centerPoint.add(diff);
                    break;
                } else {
                    return objList[closestObj].triList[closestTri].color;
                }
            }
            this->origin = this->origin.add(this->direction.times(minDist));
            if (this->origin.x < -LIMIT_X || this->origin.x > LIMIT_X) break;
            if (this->origin.y < -LIMIT_Y || this->origin.y > LIMIT_Y) break;
            if (this->origin.z < -LIMIT_Z || this->origin.z > LIMIT_Z) break;
        }
        return Color(0, 0, 0, 0); //return void color
    }
    Triangle3D::Triangle3D(Vector vertices[], Color color) {
        this->vertices[0] = vertices[0];
        this->vertices[1] = vertices[1];
        this->vertices[2] = vertices[2];
        this->color = color;
        this->centerPoint = getCenterPoint(vertices, 3);
        this->discrimRadius = getMaxDist(this->centerPoint, vertices, 3);
        //magic wizzard jizzzz (source: https://www.uninformativ.de/bin/RaytracingSchnitttests-76a577a-CC-BY.pdf)
        Vector b = vertices[1].minus(vertices[0]);
        Vector c = vertices[2].minus(vertices[0]);

        this->n = b.cross(c);
        this->n.normalize();
        this->d = this->n.dot(vertices[0]);

        float bb = b.dot(b);
        float bc = b.dot(c);
        float cc = c.dot(c);

        float D = 1.0 / (cc * bb - bc * cc);
        float bbD = bb * D;
        float bcD = bc * D;
        float ccD = cc * D;

        this->uBeta = b.times(ccD).minus(c.times(bcD));
        this->uGamma = c.times(bbD).minus(b.times(bcD));

        this->kBeta = -vertices[0].dot(this->uBeta);
        this->kGamma = -vertices[0].dot(this->uGamma);
    }
    Triangle3D::Triangle3D(stl::triangle* triangle) {
        Vector vertices[3] = {
            Vector(triangle->v1.x, triangle->v1.y, triangle->v1.z),
            Vector(triangle->v2.x, triangle->v2.y, triangle->v2.z),
            Vector(triangle->v3.x, triangle->v3.y, triangle->v3.z),
        };
        
        this->vertices[0] = vertices[0];
        this->vertices[1] = vertices[1];
        this->vertices[2] = vertices[2];
        this->color = Color(1.0, 1.0, 1.0);
        this->centerPoint = getCenterPoint(vertices, 3);
        this->discrimRadius = getMaxDist(this->centerPoint, vertices, 3);
        std::cout << "center point: x:" << this->centerPoint.x << " y:" << this->centerPoint.y << " z:" << this->centerPoint.z << " discrimRadius:" << this->discrimRadius << "\n";
        //magic wizzard jizzzz (source: https://www.uninformativ.de/bin/RaytracingSchnitttests-76a577a-CC-BY.pdf)
        Vector b = vertices[1].minus(vertices[0]);
        Vector c = vertices[2].minus(vertices[0]);

        this->n = b.cross(c);
        this->n.normalize();
        this->d = this->n.dot(vertices[0]);

        float bb = b.dot(b);
        float bc = b.dot(c);
        float cc = c.dot(c);

        float D = 1.0 / (cc * bb - bc * cc);
        float bbD = bb * D;
        float bcD = bc * D;
        float ccD = cc * D;

        this->uBeta = b.times(ccD).minus(c.times(bcD));
        this->uGamma = c.times(bbD).minus(b.times(bcD));

        this->kBeta = -vertices[0].dot(this->uBeta);
        this->kGamma = -vertices[0].dot(this->uGamma);
    }
    Triangle3D::Triangle3D(Vector vert1, Vector vert2, Vector vert3, Color color) {
        this->vertices[0] = vert1;
        this->vertices[1] = vert2;
        this->vertices[2] = vert3;
        this->color = color;
        this->centerPoint = getCenterPoint(vertices, 3);
        this->discrimRadius = getMaxDist(this->centerPoint, vertices, 3);
        //magic wizzard jizzzz (source: https://www.uninformativ.de/bin/RaytracingSchnitttests-76a577a-CC-BY.pdf)
        Vector b = vert2.minus(vert1);
        Vector c = vert3.minus(vert1);

        this->n = b.cross(c);
        this->n.normalize();
        this->d = this->n.dot(vertices[0]);

        float bb = b.dot(b);
        float bc = b.dot(c);
        float cc = c.dot(c);

        float D = 1.0 / (cc * bb - bc * cc);
        float bbD = bb * D;
        float bcD = bc * D;
        float ccD = cc * D;

        this->uBeta = b.times(ccD).minus(c.times(bcD));
        this->uGamma = c.times(bbD).minus(b.times(bcD));

        this->kBeta = -vertices[0].dot(this->uBeta);
        this->kGamma = -vertices[0].dot(this->uGamma);
    }
    Vector Triangle3D::intersectionTest(Ray* r) {
        Vector ret = r->origin;
        float rn = r->direction.dot(this->n);
        if (abs(rn) < 0.000001) return ret;

        float alpha1 = (this->d - r->origin.dot(this->n)) / rn;
        if (alpha1 <= 0.0) return ret;

        Vector q = r->evaluate(alpha1);
        float beta = this->uBeta.dot(q) + this->kBeta;
        if (beta < 0.0) return ret;

        float gamma = this->uGamma.dot(q) + this->kGamma;
        if (gamma < 0.0) return ret;

        float alpha = 1 - beta - gamma;
        if (alpha < 0.0) return ret;

        this->color = Color(beta, gamma, alpha);

        return q;
    }
    void Triangle3D::recalculate() {
        this->centerPoint = getCenterPoint(this->vertices, 3);
        Vector b = this->vertices[1].minus(this->vertices[0]);
        Vector c = this->vertices[2].minus(this->vertices[0]);

        this->n = b.cross(c);
        this->n.normalize();
        this->d = this->n.dot(vertices[0]);

        float bb = b.dot(b);
        float bc = b.dot(c);
        float cc = c.dot(c);

        float D = 1.0 / (cc * bb - bc * cc);
        float bbD = bb * D;
        float bcD = bc * D;
        float ccD = cc * D;

        this->uBeta = b.times(ccD).minus(c.times(bcD));
        this->uGamma = c.times(bbD).minus(b.times(bcD));

        this->kBeta = -vertices[0].dot(this->uBeta);
        this->kGamma = -vertices[0].dot(this->uGamma);
    }
    Color::Color() {
        this->r = 0;
        this->g = 0;
        this->b = 0;
        this->alpha = 0.0;
        this->brightness = 0.0;
    }
    Color::Color(float r, float g, float b) {
        this->r = r;
        this->g = g;
        this->b = b;
        this->alpha = 1.0;
        this->brightness = 1.0;
    }
    Color::Color(float r, float g, float b, float alpha) {
        this->r = r;
        this->g = g;
        this->b = b;
        this->alpha = alpha;
        this->brightness = 1.0;
    }
    uint8_t Color::getR() {
        return (uint8_t) (this->r * 255) % 256;
    }
    uint8_t Color::getG() {
        return (uint8_t) (this->g * 255) % 256;
    }
    uint8_t Color::getB() {
        return (uint8_t) (this->b * 255) % 256;
    }
    png::rgb_pixel Color::convertToPixel() {
        return png::rgb_pixel((int) ((float) this->r * 255) % 255, (int) ((float) this->g * 255) % 255, (int) ((float) this->b * 255) % 255);
    }
    Vector Object::calculateCenter() {
        Vector center = getCenterPoint(this);
        this->centerPoint = center;
        return center;
    }
    void Object::transform(float x, float y, float z) {
        Vector transform = Vector(x, y, z);
        for (int i = 0; i < this->nTris; i++) {
            this->triList[i].vertices[0] = this->triList[i].vertices[0].add(transform);
            this->triList[i].vertices[1] = this->triList[i].vertices[1].add(transform);
            this->triList[i].vertices[2] = this->triList[i].vertices[2].add(transform);
            this->triList[i].recalculate(); //recalculate centerpoint and angle primitives
        }
    }
    void Object::transform(Vector transform) {
        for (int i = 0; i < this->nTris; i++) {
            this->triList[i].vertices[0] = this->triList[i].vertices[0].add(transform);
            this->triList[i].vertices[1] = this->triList[i].vertices[1].add(transform);
            this->triList[i].vertices[2] = this->triList[i].vertices[2].add(transform);
            this->triList[i].recalculate();
        }
    }
    void Object::rotate(float x, float y, float z) {
        for (int i = 0; i < this->nTris; i++) {
            for (int a = 0; a < 3; a++) {
                Vector vertVec = this->triList[i].vertices[a].minus(this->centerPoint);
                vertVec.rotateX(x);
                this->triList[i].vertices[a] = vertVec.add(this->centerPoint);
            }
            for (int a = 0; a < 3; a++) {
                Vector vertVec = this->triList[i].vertices[a].minus(this->centerPoint);
                vertVec.rotateY(y);
                this->triList[i].vertices[a] = vertVec.add(this->centerPoint);
            }
            for (int a = 0; a < 3; a++) {
                Vector vertVec = this->triList[i].vertices[a].minus(this->centerPoint);
                vertVec.rotateZ(z);
                this->triList[i].vertices[a] = vertVec.add(this->centerPoint);
            }
        }
    }
    void Object::rotate(Vector rotate) {
        for (int i = 0; i < this->nTris; i++) {
            for (int a = 0; a < 3; a++) {
                Vector vertVec = this->triList[i].vertices[a].minus(this->centerPoint);
                vertVec.rotateX(rotate.x);
                this->triList[i].vertices[a] = vertVec.add(this->centerPoint);
            }
            for (int a = 0; a < 3; a++) {
                Vector vertVec = this->triList[i].vertices[a].minus(this->centerPoint);
                vertVec.rotateY(rotate.y);
                this->triList[i].vertices[a] = vertVec.add(this->centerPoint);
            }
            for (int a = 0; a < 3; a++) {
                Vector vertVec = this->triList[i].vertices[a].minus(this->centerPoint);
                vertVec.rotateZ(rotate.z);
                this->triList[i].vertices[a] = vertVec.add(this->centerPoint);
            }
        }
    }
    Object::Object(Triangle3D* triList, int nTris) {
        this->triList = triList;
        this->nTris = nTris;
        this->calculateCenter();
        this->discrimRadius = getMaxDist(this->centerPoint, this);
    }
    Object::Object() {
        this->triList = NULL;
        this->nTris = 0;
        this->centerPoint = Vector(0, 0, 0);
        this->discrimRadius = 0;
    }
    float map(float in, float min, float max, float newMin, float newMax) {
        float out = in - min;
        out = out / (max - min);
        out = out * (newMax - newMin);
        out = out + newMin;
        return out;
    }
    Vector getCenterPoint(Vector* vectors, int nVectors) {
        Vector ret = Vector(0, 0, 0);
        for (int i = 0; i < nVectors; i++) {
            ret = ret.add(vectors[i]);
        }
        ret = ret.times((float) 1.0 / nVectors);
        return ret;
    }
    Vector getCenterPoint(Object* obj) {
        Vector ret;
        for (int i = 0; i < obj->nTris; i++) {
            ret = ret.add(obj->triList[i].vertices[0]).add(obj->triList[i].vertices[1]).add(obj->triList[i].vertices[2]);
        }
        ret = ret.times((float) 1.0 / (obj->nTris * 3));
        return ret;
    }
    float getMaxDist(Vector center, Vector* vectorList, int nVectors) {
        float dist;
        for (int i = 0; i < nVectors; i++) {
            float tmp = center.minus(vectorList[i]).dist();
            if (tmp > dist) {
                dist = tmp;
            }
        }
        return dist;
    }
    float getMaxDist(Vector center, Object* obj) {
        float dist = 0;
        for (int i = 0; i < obj->nTris; i++) {
            if (float tmp = obj->triList[i].vertices[0].minus(center).dist() > dist) dist = tmp;
            if (float tmp = obj->triList[i].vertices[1].minus(center).dist() > dist) dist = tmp;
            if (float tmp = obj->triList[i].vertices[2].minus(center).dist() > dist) dist = tmp;
        }
        return dist;
    }
    float getTriDist(Vector origin, Triangle3D* triList, int nTris, bool* dontCare) {
        float dist = 9999999999;
        for (int i = 0; i < nTris; i++) {
            if (dontCare[i] == false) {
                float tmp = origin.minus(triList[i].centerPoint).dist() - triList[i].discrimRadius;
                if (tmp < dist) {
                    dist = tmp;
                    closestTri = i;
                }
            }
        }
        return dist;
    }
}