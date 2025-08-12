#include <fstream>
#include <sstream>
#include <string>
#include "model.h"

Model::Model(const char* fileName){
    std::ifstream f(fileName);
    
    if (f.fail())
        return;

    std::string s;
    while(!f.eof()){
        std::getline(f, s);

        std::stringstream ss(s);
        std::string prefix;
        ss>>prefix;

        if(prefix == "v"){
            vec3f v;
            ss>>v.x>>v.y>>v.z;
            verts.push_back(v);
        }
        
        if(prefix == "vt"){
            vec3f vtx(0, 0, 0);
            ss>>vtx.x>>vtx.y;
            text_verts.push_back(vtx);
        }

        if (prefix == "f") {
            // std::cout<<"check"<<std::endl;
            int ver, tex, normal;
            std::vector<int> face_data;
            
            char slash;
            // This loop handles the "f v/t/n v/t/n v/t/n" format
            while(ss>>ver>>slash>>tex>>slash>>normal){ //follow the obj file format
                face_data.push_back(ver-1);
                face_data.push_back(tex-1);
                face_data.push_back(normal-1);
            }

            faces.push_back(face_data);
            
        }
    }

    auto load_texture = [&fileName](const std::string suffix, TGAImage& img ){
        std::string name(fileName);
        size_t dot = name.find_last_of(".");
        if (dot == std::string::npos)
            return;
        std::string textfile = name.substr(0, dot) + suffix;
        std::cerr << "texture file" << textfile << " loading " << (img.read_tga_file(textfile.c_str())? "ok":"failed")<<std::endl; 
    };

    load_texture("_diffuse.tga", diffusemap);
    diffusemap.flip_vertically();
}

// const TGAImage& Model::diffuse() const{
//     return diffusemap;
// }
vec2f Model::uv(int iface, int nvert){
    int index = faces[iface][nvert * 3 + 1];
    vec3f v = text_verts[index];
    return vec2f(v.x, v.y);
}

TGAColor Model::diffuse (vec2f uv){
    double u = uv.x * diffusemap.get_width();
    double v = uv.y * diffusemap.get_height();
    return diffusemap.get(u, v);
} 

Model::~Model(){}

int Model::num_verts(){
    return verts.size();
}

int Model::num_faces(){
    return faces.size();
}

vec3f Model::vert(int i){
    return verts[i];
}

vec3f Model::text_vert(int i){
    return text_verts[i];    
}

std::vector<int> Model::face(int i){
    return faces[i];
}
