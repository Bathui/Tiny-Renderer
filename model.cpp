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

        std::stringstream ss;
        ss<<s;

        char trash;

        ss>>trash; // mode character v and f or '/' is useless in storing to the data structure
        if(s[0] == 'v'&&s[1] == ' '){
            vec3f v;
            ss>>v.x>>v.y>>v.z;
            verts.push_back(v);
        }
        
        if(s[0] == 'v' && s[1] == 't' && s[2] == ' '){
            vec3f vtx;
            ss>>vtx.x>>vtx.y>>vtx.z;
            text_verts.push_back(vtx);
        }

        if (s[0] == 'f' && s[1] == ' ') {
            // std::cout<<"check"<<std::endl;
            int ver, tex, normal;
            std::vector<int> vec;
            
            while(ss>>ver>>trash>>tex>>trash>>normal){ //follow the obj file format
                vec.push_back(ver-1);
                vec.push_back(tex-1);
                vec.push_back(normal-1);
            }

            faces.push_back(vec);
            
        }
    }
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
