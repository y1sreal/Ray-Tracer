#ifndef TRIANGLEH
#define TRIANGLEH

#include "hitable.h"
// the following is adopted from my partner, Wang Jingjin
class triangle : public hitable {
public:
    triangle() {}
    /**
     * For a triangle facing the camera, p0, p1, p2 should be passed into the constructor in a counterclockwise order
     * @param p0 the coordinate of an angle
     * @param p1 the coordinate of an angle
     * @param p2 the coordinate of an angle
     * @param mat the material of the triangle
     */
    triangle(vec3 p0, vec3 p1, vec3 p2, material *mat) :
            p0(p0), p1(p1), p2(p2), mp(mat) {
        vec3 p0p1 = p1 - p0;
        vec3 p1p2 = p2 - p1;

        N = unit_vector(cross(p0p1, p1p2));
    };
    virtual bool hit(const ray& r, float t0, float t1, hit_record& rec) const;
    material *mp;
    vec3 p0, p1, p2;
    vec3 N;
};

/**
 * reference : https://www.scratchapixel.com/code.php?id=9&origin=/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle
 */
bool triangle::hit(const ray& r, float t0, float t1, hit_record& rec) const {
    vec3 p0p1 = p1 - p0;
    vec3 p0p2 = p2 - p0;
    vec3 pvec = cross(r.direction(), p0p2);
    float det = dot(p0p1, pvec);

    if (det < 0.001 && det > -0.001) {
        return false;
    }

    float invDet = 1.0 / det;
    vec3 tvec = r.origin() - p0;
    float u = dot(tvec, pvec) * invDet;
    if (u < 0 || u > 1) {
        return false;
    }

    vec3 qvec = cross(tvec, p0p1);
    float v = dot(r.direction(), qvec) * invDet;
    if (v < 0 || u + v > 1) {
        return false;
    }

    float t = dot(p0p2, qvec) * invDet;
    if (t < t0 || t > t1) {
        return false;
    }
    rec.t = t;
    rec.p = r.point_at_parameter(t);
    rec.normal = N;
    rec.mat_ptr = mp;

    return true;
}

#endif
