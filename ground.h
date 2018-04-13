#ifndef GROUNDH
#define GROUNDH

#include "hitable.h"
// the following is adopted from my partner, Wang Jingjin and modified by Tang Yiliu
class ground : public hitable {
public:
    ground() {}
    ground(float height, material* mp) : height(height), mp(mp) {}
    virtual bool hit(const ray&r, float t0, float t1, hit_record& rec) const;
    material* mp;
    float height;
};

bool ground::hit(const ray& r, float t0, float t1, hit_record &rec) const {
    float temp = height / r.direction().y();
    if (temp < t1 && temp > t0) {
        rec.t = temp;
        rec.p = r.point_at_parameter(temp);
        rec.normal = vec3(0,1,0);
        rec.mat_ptr = mp;
        return true;
    }
    return false;
}

#endif
