#ifndef MODEL_H
#define MODEL_H
#include <vector>
#include "geometry.h"
#include "tgaimage.h"
class Model
{
    private:
        std::vector<vec3f> verts;
        std::vector<vec3f> text_verts;
        std::vector<vec3f> norms;
        std::vector<std::vector<int>> faces;
        
        TGAImage diffusemap = {};
        TGAImage specularmap = {};
        TGAImage normalmap = {};       

    public:
        Model(const char* fileName);
        ~Model();

        int num_verts();
        int num_faces();

        vec2f uv(int iface, int nvert);
        vec3f vert(int i);
        vec3f text_vert(int i);
        vec3f norm (int iface, int nvert);
        
        std::vector<int> face(int i);
        // const TGAImage& diffuse() const;

        TGAColor diffuse(vec2f uv);
        TGAColor normal_Map(vec2f uv);
        TGAColor specular(vec2f uv);
};
#endif  