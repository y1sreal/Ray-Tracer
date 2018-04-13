#ifndef MATERIALH
#define MATERIALH

struct hit_record;

#include "ray.h"
#include "hitable.h"
// the following is adopted from Ray Tracing in One Weekend and modified by my
// partner, Wang jingjin and me

float schlick(float cosine, float ref_idx) {
    float r0 = (1-ref_idx) / (1+ref_idx);
    r0 = r0*r0;
    return r0 + (1-r0)*pow((1 - cosine),5);
}

bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted) {
    vec3 uv = unit_vector(v);
    float dt = dot(uv, n);
    float discriminant = 1.0 - ni_over_nt*ni_over_nt*(1-dt*dt);
    if (discriminant > 0) {
        refracted = ni_over_nt*(uv - n*dt) - n*sqrt(discriminant);
        return true;
    }
    else
        return false;
}



vec3 reflect(const vec3& v, const vec3& n) {
     return v - 2*dot(v,n)*n;
}


vec3 random_in_unit_sphere() {
    vec3 p;
    do {
        p = 2.0*vec3(drand48(),drand48(),drand48()) - vec3(1,1,1);
    } while (p.squared_length() >= 1.0);
    return p;
}


class material  {
    public:
        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
        material(const vec3& a) : diffuse_albedo(a) {
            specular_albedo = vec3(0,0,0);
            shininess = 1;
            ambient = vec3(0, 0, 0);
        }
        material(const vec3& a, const vec3& b) : diffuse_albedo(a), specular_albedo(b) {
            shininess = 1;
            ambient = vec3(0,0,0);
        }
        material(const vec3& a, const vec3 &b, const float &c) : diffuse_albedo(a), specular_albedo(b), shininess(c) {
            ambient = vec3(0, 0, 0);
        }
        material(const vec3& a, const vec3 &b, const float &c, const vec3 &d) : diffuse_albedo(a), specular_albedo(b), shininess(c), ambient(d) {}

        vec3 diffuse_albedo;
        vec3 specular_albedo;
        float shininess;
        vec3 ambient;
};

class lambertian : public material {
    public:
        lambertian(const vec3& a) : material(a) {}
        lambertian(const vec3& a, const vec3 &b) : material(a, b) {}
        lambertian(const vec3& a, const vec3 &b, const float &c) : material(a, b, c) {}
        lambertian(const vec3& a, const vec3 &b, const float &c, const vec3 &d) : material(a, b, c, d) {}
        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const  {
             vec3 target = rec.p + rec.normal + random_in_unit_sphere();
             scattered = ray(rec.p, target-rec.p);
             attenuation = diffuse_albedo;
             return true;
        }
};

class metal : public material {
    public:
        metal(const vec3& a, float f) : material(a) { if (f < 1) fuzz = f; else fuzz = 1; }
        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const  {
            vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
            scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());
            attenuation = diffuse_albedo;
            return (dot(scattered.direction(), rec.normal) > 0);
        }
        float fuzz;
};

class dielectric : public material {
    public:
        dielectric(float ri) : ref_idx(ri), material(vec3(1.0, 1.0, 1.0)) {}
        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const  {
             vec3 outward_normal;
             vec3 reflected = reflect(r_in.direction(), rec.normal);
             float ni_over_nt;
             attenuation = vec3(1.0, 1.0, 1.0);
             vec3 refracted;
             float reflect_prob;
             float cosine;
             if (dot(r_in.direction(), rec.normal) > 0) {
                  outward_normal = -rec.normal;
                  ni_over_nt = ref_idx;
                  cosine = ref_idx * dot(r_in.direction(), rec.normal) / r_in.direction().length();
        //          cosine = dot(r_in.direction(), rec.normal) / r_in.direction().length();
        //          cosine = sqrt(1 - ref_idx*ref_idx*(1-cosine*cosine));
             }
             else {
                  outward_normal = rec.normal;
                  ni_over_nt = 1.0 / ref_idx;
                  cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
             }
             if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted))
                reflect_prob = schlick(cosine, ref_idx);
             else
                reflect_prob = 1.0;
             if (drand48() < reflect_prob)
                scattered = ray(rec.p, reflected);
             else
                scattered = ray(rec.p, refracted);
             return true;
        }

        float ref_idx;
};

#endif
