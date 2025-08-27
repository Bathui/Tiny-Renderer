#ifndef RENDERING_H
#define RENDERING_H
#include "tgaimage.h"
#include "model.h"

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);

vec3f baryCentric(vec3f* vertices, vec3f p);

class Shader{
    public:
        float* varing_intensity = new float [3];
        vec3f* normals = new vec3f[3];
        vec3f* ndc_coord = new vec3f[3];

        virtual vec3f vertex_shader(int iface, int nthvert, Matrix projection, Matrix model_view, Matrix ViewPort) = 0;
        // virtual 
        
        virtual bool fragment(vec2f uvP, vec3f nmA, vec3f nmB, float* zbuffer, vec3f P, int idx, float phi, TGAColor& color, vec3i screen[3], vec2f uv0, vec2f uv1, vec2f uv2) = 0;

        virtual ~Shader(){
            delete[] varing_intensity;
            delete[] normals;
            delete[] ndc_coord;
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

vec3f new_basis(Matrix& Ainv, float f0, float f1, float f2);

vec3f Darboux2World(vec3f new_x, vec3f new_y, vec3f new_z, vec3f normal_coord);

#endif