#ifndef RENDERING_H
#define RENDERING_H
#include "tgaimage.h"
#include "model.h"
void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);

vec3f baryCentric(vec3f* vertices, vec3f p);

void triangle(vec3f* pts, float* zbuffer, TGAImage& img, TGAColor color);

#endif
