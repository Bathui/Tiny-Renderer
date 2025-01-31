#ifndef MODEL_H
#define MODEL_H
#include <vector>
#include "geometry.h"

class Model
{
    private:
        std::vector<vec3f> verts;
        std::vector<std::vector<int>> faces;
    public:
        Model(const char* fileName);
        ~Model();

        int num_verts();
        int num_faces();

        vec3f vert(int i);
        std::vector<int> face(int i);
};
#endif  