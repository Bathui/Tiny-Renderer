#include "rendering.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const int width = 2000;
const int height = 2000;

int main(int argc, char** argv) {
	const char* name = nullptr;
	if (argc == 1){
		name = "output.tga";
	}
	else{
		name = argv[1];
	}
	TGAImage image(width, height, TGAImage::RGB);
	Model* model = new Model("african_head.obj");
	// Model* model = new Model("sample.obj");
	// std::cout<<model->num_faces()<<std::endl;
    std::cout<<model->num_verts()<<std::endl;
	float* zbuffer = new float[width*height];
	//set default values for z-buffer
	for (int i=width*height; i>=0; i--)
		zbuffer[i] = -std::numeric_limits<float>::max();
    vec3f light_direction(0, 0, -1);

    for (int i = 0; i < model->num_faces(); i++) {
        vec3f world[3];
        vec3f screen[3];
        std::vector<int> face = model->face(i);

        for(int j = 0; j < 3; j++) {
            world[j] = model->vert(face[3*j]);
            screen[j] = vec3f(int((world[j].x + 1.) * width / 2 + 0.5), int((world[j].y + 1.) * height / 2+0.5), world[j].Z());  
        }
        
		vec3f normal = (world[2] - world[0]) ^ (world[1] - world[0]);
        normal.normalized();
        float intensity = normal * light_direction;

        if(intensity > 0)
            triangle(screen, zbuffer, image, TGAColor(255*intensity, 255*intensity, 255*intensity, 255));
        
    }
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file(name);
	delete[] zbuffer;
	delete model;
}

