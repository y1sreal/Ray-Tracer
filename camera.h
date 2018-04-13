#ifndef CAMERAH
#define CAMERAH

#include "ray.h"
// the following is adopted from Ray Tracing in One Weekend
class camera {
public:
    camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect) { // vfov is top to bottom in degrees
        vec3 u, v, w;
        float theta = vfov*M_PI/180;
        float half_height = tan(theta/2);
        float half_width = aspect * half_height;
        origin = lookfrom;
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);
        lower_left_corner = origin - half_width*u - half_height*v - w;
        horizontal = 2*half_width*u;
        vertical = 2*half_height*v;
    }
// the following is adopted from my partner Wang jingjin 
    camera() {
        lower_left_corner = vec3(-2.0, -1.0, -1.0);
        horizontal = vec3(4.0, 0.0, 0.0);
        vertical = vec3(0.0, 2.0, 0.0);
        origin = vec3(0.0, 0.0, 0.0);
    }

    virtual ray get_ray(float u, float v) {
        return ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
    }

    virtual vec3 get_origin(vec3 &p) const {
        return origin;
    }

    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 u, v, w;
};

class orthographic_camera : public camera {
public:
    orthographic_camera() : camera() {}
    ray get_ray(float u, float v) const {
        vec3 to = lower_left_corner + u*horizontal + v*vertical;
        vec3 from = vec3(to.x(), to.y(), 0);
        vec3 dir = vec3(0, 0, to.z());
        return ray(from, dir);
    }
    vec3 get_origin(vec3& p) const {
        return vec3(p.x(), p.y(), 0.0);
    }
};

#endif
