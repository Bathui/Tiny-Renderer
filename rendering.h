#ifndef RENDERING_H
#define RENDERING_H
#include "tgaimage.h"
#include "model.h"

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);

vec3f baryCentric(vec3f* vertices, vec3f p);

void rasterize(vec3f* pts, float* zbuffer, float intensity, TGAImage& img, vec2f* tex_coords, TGAImage& texture, Model& model);

vec3f m2v(Matrix m);

Matrix v2m(vec3f& v);

Matrix viewport(int x, int y, int w, int h, int d);
#endif
