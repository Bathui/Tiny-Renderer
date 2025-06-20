#ifndef RENDERING_H
#define RENDERING_H
#include "tgaimage.h"
#include "model.h"

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);

vec3f baryCentric(vec3f* vertices, vec3f p);

void rasterize(vec3f* pts, float* zbuffer, float intensity, TGAImage& img, vec2f* tex_coords, TGAImage& texture, Model& model);

struct Shader{
    static TGAColor sample2D (TGAImage& img, vec2f &uvf){
        return img.get(uvf.x * img.get_width(), uvf.y* img.get_height());
    }

};
#endif
