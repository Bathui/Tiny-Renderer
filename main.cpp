#include "rendering.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const int width = 2000;
const int height = 2000;
const int depth = 255;
vec3f camera(0, 0, 3); // set our camera

int main(int argc, char** argv) {
	const char* name = nullptr;
	if (argc == 1){
		name = "output.tga";
	}
	else{
		name = argv[1];
	}
	TGAImage image(width, height, TGAImage::RGB);
	Model* model = new Model("texture/african_head.obj");// african_head_diffuse
	// a new change
	TGAImage texture;
	texture.flip_vertically();

    std::cout<<model->num_verts()<<std::endl;
	float* zbuffer = new float[width*height];
	//set default values for z-buffer
	for (int i=width*height-1; i>=0; i--)
		zbuffer[i] = -std::numeric_limits<float>::max();
    vec3f light_direction(0, 0, -1);

	Matrix projection = Matrix::identity(4);
	Matrix ViewPort = viewport(width/8, height/8, width*3/4, height*3/4, depth); // scale to the place where our eyes can see 
    projection[3][2] = -1.f/camera.z;

	for (int i = 0; i < model->num_faces(); i++) {
        vec3f world[3];
        vec3f screen[3];
		//might have mistakes from here
		vec2f uvs[3];
        std::vector<int> face = model->face(i);
		
        for(int j = 0; j < 3; j++) {
			
			// tex_coord[j].x *= texture.get_width();
			// tex_coord[j].y *= texture.get_height();

            world[j] = model->vert(face[3*j]);
            screen[j] = vec3f(int((world[j].x + 1.) * width / 2 + 0.5), int((world[j].y + 1.) * height / 2+0.5), world[j].Z());  
			uvs[j] = model->uv(i, j);	
		}
		
        
		vec3f normal = (world[2] - world[0]) ^ (world[1] - world[0]);
        normal.normalized();
        float intensity = normal * light_direction;
		
		
        if(intensity > 0){
			
			rasterize(screen, zbuffer, intensity, image, uvs, texture, *model);
		}
    }
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file(name);
	delete[] zbuffer;
	delete model;
}

