#include <iostream>
#include <fstream>
#include "sphere.h"
#include "hitablelist.h"
#include "float.h"
#include "camera.h"
#include "material.h"
#include "plane.h"
#include "triangle.h"
#include "ground.h"
#include <cfloat>

using namespace std;
// the following is adopted from Ray Tracing in One Weekend
vec3 color(const ray& r, hitable *world, int depth) {
        hit_record rec;
        if (world->hit(r, 0.0, FLT_MAX, rec)) {
            ray scattered;
            vec3 attenuation;
            if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
                return attenuation*color(scattered, world, depth+1);
            } else {
                return vec3(1,0,0);
            }
        } else {
            vec3 unit_direction = unit_vector(r.direction());
            float t = 0.5 * (unit_direction.y() + 1.0);
            return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
        }
}
// the following is adopted from my partner, Wang Jingjin and modified by Tang Yiliu
vec3 color_phong(const camera & cam, ray r, vec3 dir, hitable *world, vec3 light_intensity) {
    hit_record rec;
    if (world->hit(r, 0.001, FLT_MAX, rec)) {
        vec3 diffuse_albedo = rec.mat_ptr->diffuse_albedo;
        vec3 specular_albedo = rec.mat_ptr->specular_albedo;
        float shininess = rec.mat_ptr->shininess;
        vec3 ambient = rec.mat_ptr->ambient;
        vec3 L = light_intensity * unit_vector(-dir);
        vec3 N = rec.normal;
        vec3 V = unit_vector(cam.get_origin(rec.p) - rec.p);
        vec3 R = reflect(dir, rec.normal);

        bool blocked = world->hit(ray(rec.p, -dir), 0.001, FLT_MAX, rec);
        vec3 diffuse = blocked ? vec3(0.0, 0.0, 0.0) : max<float>(dot(N, L), 0) * diffuse_albedo;
        vec3 specular = blocked ? vec3(0.0, 0.0, 0.0) : pow(max<float>(dot(R, V), 0), shininess) * specular_albedo;
        vec3 color = ambient + diffuse + specular;
//        vec3 sig_color = vec3(1.0/(1.0 + exp(-color.x())),
//                              1.0/(1.0 + exp(-color.y())),
//                              1.0/(1.0 + exp(-color.z())));
        vec3 cap_color = vec3(color.x() > 1.0 ? 1.0 : color.x(),
                                color.y() > 1.0 ? 1.0 : color.y(),
                                color.z() > 1.0 ? 1.0 : color.z());
        return cap_color;
    }
    return vec3(0, 0, 0);
}



void perspective_projection_phong(camera &cam, hitable *world, int nx, int ny, int ns, vec3 dir, ofstream &output) {
    output << "P3\n" << nx << " " << ny << "\n255\n";
    for (int j = ny-1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            vec3 col(0, 0, 0);
            for (int s = 0; s < ns; s++) { // jittered sampling
                float u = float(i + drand48()) / float(nx);
                float v = float(j + drand48()) / float(ny);
                ray r = cam.get_ray(u, v);
//                col += color_shirley(r, world, 0);
                col += color_phong(cam, r, dir, world, vec3(1.0, 1.0, 1.0));
            }
            col /= float(ns);
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));

            int ir = int(255.99 * col[0]);
            int ig = int(255.99 * col[1]);
            int ib = int(255.99 * col[2]);

            output << ir << " " << ig << " " << ib << "\n";
        }
    }
    output.close();
}

void orthographic_projection(hitable * world, int nx, int ny, int ns, vec3 dir, ofstream& output) {
    orthographic_camera cam;
    perspective_projection_phong(cam, world, nx, ny, ns, dir, output);
}

int main() {
    ofstream output;
    output.open("perspevtive.ppm");
    int nx = 400;
    int ny = 200;
    int ns = 100;
    hitable *list[5];
    list[0] = new sphere(vec3(0,0,-1), 0.5, new lambertian(vec3(0.5, 0.6, 0.4), vec3(0.8, 0.7, 0.6),  5, vec3(0.09, 0.09, 0.09)));
//    list[1] = new sphere(vec3(0,-100.5,-1), 100, new dielectric(0.1));
    list[1] = new ground(-1, new lambertian(vec3(0.4, 0.6, 0.3), vec3(1,0.8,0.6), 5, vec3(0.07, 0.08, 0.09)));
    list[2] = new triangle(vec3(-4, 0.1, -4), vec3(-1, 0.1, -2), vec3(-2, 4, -4), new lambertian(vec3(0.5, 0.3, 0.8), vec3(0.5,0.2,0.9), 5, vec3(0.07, 0.05, 0.03)));
    list[3] = new triangle(vec3(-1, 0.1, -2), vec3(1, 0.1, -1), vec3(0, 1, -1), new lambertian(vec3(0.2, 0.6, 0.8), vec3(0.7,0.7,0.6), 5, vec3(0.03, 0.06, 0.15)));
    list[4] = new sphere(vec3(-1.5, -0.5, -2), 0.6, new lambertian(vec3(0.8, 0.3, 0.3), vec3(0.7,0.7,0.6), 5, vec3(0.08, 0.03, 0.03)));
//    list[2] = new sphere(vec3(1,0,-1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 1.0));
//    list[3] = new sphere(vec3(-1,0,-1), 0.5, new metal(vec3(0.8, 0.8, 0.8), 0.0));
//    list[4] = new xyplane(-1, 1, 0.3, 1.3, -1, new metal(vec3(0.8, 0.3, 0.3), 0.5));
//    list[4] = new triangle(vec3(-1,0,-1),vec3(1,0,-1),vec3(0,1,-1),new lambertian(vec3(0.8, 0.3, 0.3)));

    // use negative radius dielectric sphere to make some decorative bubbles!

    hitable *world = new hitable_list(list, 5);
    camera cam(vec3(1, 0, -2), vec3(0, 0, -1), vec3(0, 1, 0), 90, float(nx)/float(ny));
    vec3 light_dir(1, 0, -0.15);

// uncomment the following line and comment the line following to use orthographic mode
// also commnet the camera cam initilization

//    orthographic_projection(world, nx, ny, ns, light_dir, output);
    perspective_projection_phong(cam, world, nx, ny, ns, light_dir, output);
    return 0;
}
