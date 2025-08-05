#include "rendering.h"
#include <cstring>
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

const int width = 2000;
const int height = 2000;
const int depth = 255;

vec3f camera(0.25, 0.25, 0.75); // set our camera
vec3f center(0, 0, -2);
vec3f light_direction = vec3f(1,-1,1).normalized();
vec3f light_source = vec3f(100,100,100);

// const float diffusion_coef = 0.75;

int main(int argc, char** argv) {
	// add a cml interface for the rendering options
	// 1. Default option is african head 2. If there is a specific choice, use that
	const char* name = nullptr;
	const char* specified_obj = nullptr;
	char prefix[128] = "texture/";
	
	Model* model = nullptr;

	if (argc == 1){
		name = "output.tga";
		model = new Model("texture/african_head.obj");// african_head_diffuse
	}
	else{
		name = argv[1];
		if (argc == 3){
			specified_obj = argv[2];
			strcat(prefix, specified_obj);
			model = new Model(prefix);
		}
		else
			model = new Model("texture/african_head.obj");
	}
	TGAImage image(width, height, TGAImage::RGB);
	
	

    std::cout<<model->num_verts()<<std::endl;
	float* zbuffer = new float[width*height];
	//set default values for z-buffer
	for (int i=width*height-1; i>=0; i--)
		zbuffer[i] = -std::numeric_limits<float>::max();

	Matrix projection = Matrix::identity(4);
	Matrix ViewPort = viewport(width/8, height/8, width*3/4, height*3/4, depth); // scale to the place where our eyes can see 
    projection[3][2] = -1.f/(camera - center).norm();

	Matrix model_view = move_camera(camera, center, vec3f(0, 1, 0));
	Matrix M = ViewPort * projection * model_view;

	for (int i = 0; i < model->num_faces(); i++) {
        vec3f world[3];
        vec3f screen[3];
		vec2f uvs[3];
        float intensity[3];
		std::vector<int> face = model->face(i);
		
        for(int j = 0; j < 3; j++) {
            world[j] = model->vert(face[3*j]);
            screen[j] = m2v(M*v2m(world[j]));  
			uvs[j] = model->uv(i, j);	

			float dist = light_source.distance(screen[j]);
			intensity[j] = std::max(model->norm(i, j) * light_direction, 0.f);
			
		}
		
        //Followings are for the flat shading
		// vec3f normal = (world[2] - world[0]) ^ (world[1] - world[0]);
        // normal.normalized();
		// float flat_intensity = normal * light_direction;
		
		rasterize(screen[0], screen[1], screen[2], uvs[0], uvs[1], uvs[2], intensity[0], intensity[1], intensity[2], zbuffer, model, image);
		
    }
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file(name);
	delete[] zbuffer;
	delete model;
}
