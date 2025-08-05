#ifndef RENDERING_H
#define RENDERING_H
#include "tgaimage.h"
#include "model.h"

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);

vec3f baryCentric(vec3f* vertices, vec3f p);

void rasterize(vec3i t0, vec3i t1, vec3i t2, vec2f uv0, vec2f uv1, vec2f uv2, float it0, float it1, float it2, float *zbuffer, Model* model, TGAImage& img);

vec3f m2v(Matrix m);

Matrix v2m(vec3f& v);

Matrix viewport(int x, int y, int w, int h, int d);

Matrix move_camera (vec3f eye, vec3f center, vec3f up) ;

#endif
