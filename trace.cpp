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
    Triangle3D::Triangle3D(Vector vertices[], Color color) {
        this->color = color;
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
        
        this->color = Color(1.0, 1.0, 1.0);
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
    Vector Triangle3D::intersectionTest(Ray r) {
        Vector ret = r.origin;
        float rn = r.direction.dot(this->n);
        if (abs(rn) < 0.000001) return ret;

        float alpha1 = (this->d - r.origin.dot(this->n)) / rn;
        if (alpha1 <= 0.0) return ret;

        Vector q = r.evaluate(alpha1);
        float beta = this->uBeta.dot(q) + this->kBeta;
        if (beta < 0.0) return ret;

        float gamma = this->uGamma.dot(q) + this->kGamma;
        if (gamma < 0.0) return ret;

        float alpha = 1 - beta - gamma;
        if (alpha < 0.0) return ret;

        this->color = Color(beta, gamma, alpha);

        return q;
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
    float map(float in, float min, float max, float newMin, float newMax) {
        float out = in - min;
        out = out / (max - min);
        out = out * (newMax - newMin);
        out = out + newMin;
        return out;
    }
}