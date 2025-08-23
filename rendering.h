#ifndef RENDERING_H
#define RENDERING_H
#include "tgaimage.h"
#include "model.h"

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);

vec3f baryCentric(vec3f* vertices, vec3f p);

class Shader{
    public:
        float* varing_intensity = new float [3];
        virtual vec3f vertex_shader(int iface, int nthvert, vec3f normal_vert, Matrix projection, Matrix model_view, Matrix ViewPort) = 0;
        // virtual 
        
        virtual bool fragment(float* zbuffer, vec3f P, int idx) = 0;

        virtual ~Shader(){
            delete[] varing_intensity;
        }
};

// void rasterize(vec3f screen[3], vec2f uv0, vec2f uv1, vec2f uv2, Shader& shader, float *zbuffer, Model* model, TGAImage& img);
// void rasterize(vec3f screen[3], vec2f uv0, vec2f uv1, vec2f uv2, float it0, float it1, float it2, Shader& shader, float *zbuffer, Model* model, TGAImage& img);
void rasterize(vec3i screen[3], vec2f uv0, vec2f uv1, vec2f uv2, Shader& shader, float *zbuffer, Model* model, TGAImage& img);
vec3f m2v(Matrix m);

Matrix v2m(vec3f& v);

Matrix viewport(int x, int y, int w, int h, int d);

Matrix move_camera (vec3f eye, vec3f center, vec3f up) ;

vec3f color2Vec3(TGAColor& color);


#endif
