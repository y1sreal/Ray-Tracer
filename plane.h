#ifndef PLANEH
#define PLANEH

#include "hitable.h"
// the following is adopted from Ray Tracing in One Weekend and modified by Tang Yiliu
class xyplane : public hitable {
public:
    xyplane() {}
    xyplane(float x0, float x1, float y0, float y1, float k, material* mat) :
    x0(x0), x1(x1), y0(y0), y1(y1), k(k), mp(mat) {};
    virtual bool hit(const ray& r, float t0, float t1, hit_record& rec) const;
    material *mp;
    float x0, x1, y0, y1, k;
};

bool xyplane::hit(const ray& r, float t0, float t1, hit_record& rec) const {
    float t = (k-r.origin().z()) / r.direction().z();
    if (t < t0 || t > t1) {
        return false;
    }
    float x = r.origin().x() + t*r.direction().x();
    float y = r.origin().y() + t*r.direction().y();
    if (x < x0 || x > x1 || y < y0 || y > y1) {
        return false;
    }
//   rec.u = (x-x0)/(x1-x0);
//    rec.v = (y-y0)/(y1-y0);
    rec.t = t;
    rec.mat_ptr = mp;
    rec.p = r.point_at_parameter(rec.t);
    rec.normal = vec3(0,0,-1);
    return true;
}


#endif
