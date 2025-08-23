#include "rendering.h"
#include <cstring>
/**
 * Mark of getting new basis
 * 1. we do not need to use the normal from the original obj. Just use the one from the new normal mapping tga
 */
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

const int width = 2000;
const int height = 2000;
const int depth = 255;

vec3f camera(0.25, 0.25, 0.75); // set our camera
vec3f center(0, 0, -2);
vec3f light_direction = vec3f(0,0,2).normalized();
vec3f light_source = vec3f(100,100,100);

Model* model = nullptr;

class GouraudShader : public Shader{
	public:

		vec3f vertex_shader(int iface, int nthvert, vec3f normal_coord, Matrix projection, Matrix model_view, Matrix ViewPort){

			varing_intensity[nthvert] = std::max(normal_coord * light_direction, 0.f);
			
			std::vector<int> face = model->face(iface);
			vec3f world = model->vert(face[3*nthvert]);
			
			Matrix M = ViewPort * projection * model_view;
			vec3f screen = m2v(M * v2m(world));
			
			return screen;
		}
		bool fragment(float* zbuffer, vec3f P, int idx){
			if (zbuffer[idx] < P.z)	
				return false;	
			return true;
		}
		
		// vec3f norm_Map(vec3f surface_norm, vec3f& p0, vec3f& p1, vec3f& p2, vec3f& b){
		// 	Matrix A = Matrix::identity(3);
			

		// 	vec3f p0p1 = p1 - p0;
		// 	vec3f p1p2 = p2 - p1;
		// 	vec3f p0p2 = p2 - p0;

		// 	A[0][0] = p0p1.x; A[0][1] = p0p1.y; A[0][2] = p0p1.z;
		// 	A[1][0] = p0p2.x; A[1][1] = p0p2.y; A[1][2] = p0p2.z;
		// 	A[2][0] = surface_norm.x; A[2][1] = surface_norm.y; A[2][2] = surface_norm.z;

		// 	Matrix A_inverse = A.inverse();
		// 	Matrix res = A_inverse * v2m(b);
			
		// 	return vec3f(res[0][0], res[1][0], res[2][0]);
		// }
};

// const float diffusion_coef = 0.75;

int main(int argc, char** argv) {
	// add a cml interface for the rendering options
	// 1. Default option is african head 2. If there is a specific choice, use that
	const char* name = nullptr;
	const char* specified_obj = nullptr;
	char prefix[128] = "texture/";
	

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
	GouraudShader shader; 
	
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
        vec3i screen[3];
		vec2f uvs[3];
		std::vector<int> face = model->face(i);
		
		Matrix A = Matrix::identity(3);
        for(int j = 0; j < 3; j++) {
            world[j] = model->vert(face[3*j]);
            // screen[j] = m2v(M*v2m(world[j]));  
			uvs[j] = model->uv(i, j);	
			// float dist = light_source.distance(screen[j]);
			TGAColor normal_color = model->normal_Map(uvs[j]);
			vec3f normal_coord = color2Vec3(normal_color);

			screen[j] = shader.vertex_shader(i, j, normal_coord, projection, model_view, ViewPort);
			
			
		}
		
        //Followings are for the flat shading
		// vec3f normal = (world[2] - world[0]) ^ (world[1] - world[0]);
        // normal.normalized();
		// float flat_intensity = normal * light_direction;
		
		rasterize(screen, uvs[0], uvs[1], uvs[2], shader, zbuffer, model, image);
		
    }
	image.flip_vertically(); 
	image.write_tga_file(name);
	delete[] zbuffer;
	delete model;
}
