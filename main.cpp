#include "rendering.h"
#include <cstring>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

const int width = 2000;
const int height = 2000;
const int depth = 255;

const double diffusion_coeff =  1.3;

vec3f camera(0.25, 0.25, 0.75); // set our camera
vec3f center(0, 0, -2);
vec3f light_direction = vec3f(1,1,1).normalized();
vec3f light_source = vec3f(100,100,100);

Model* model = nullptr;

class GouraudShader : public Shader{
	public:

		vec3f vertex_shader(int iface, int nthvert, Matrix projection, Matrix model_view, Matrix ViewPort){

			std::vector<int> face = model->face(iface);
			vec3f world = model->vert(face[3*nthvert]);
			
			Matrix M = ViewPort * projection * model_view;
			vec3f screen = m2v(M * v2m(world)); // we can divide that w at the end because viewport does not affect w
			ndc_coord[nthvert] = m2v(projection * model_view * v2m(world));
			return screen;
		}
		bool fragment(vec2f uvP, vec3f nmA, vec3f nmB, float* zbuffer, vec3f P, int idx, float phi,  TGAColor& color, vec3i screen[3], vec2f uv0, vec2f uv1, vec2f uv2){	
			if (zbuffer[idx] >= P.z)	
				return true;
			TGAColor normal_color = model->normal_Map(uvP);
			vec3f p0p1 = ndc_coord[1] - ndc_coord[0];
			vec3f p0p2 = ndc_coord[2] - ndc_coord[0];
			Matrix A = Matrix::identity(3);
			A[0][0] = p0p1.x; A[0][1] = p0p1.y; A[0][2] = p0p1.z;
			A[1][0] = p0p2.x; A[1][1] = p0p2.y; A[1][2] = p0p2.z;

			vec3f nmP = nmA +(nmB - nmA) * phi;
			A[2][0] = nmP.x; A[2][1] = nmP.y; A[2][2] = nmP.z;

			Matrix Ainv = A.inverse();
			vec3f new_x = new_basis(Ainv, uv0.x, uv1.x, uv2.x);
			vec3f new_y = new_basis(Ainv, uv0.y, uv1.y, uv2.y);
			vec3f new_z = nmP;

			vec3f real_normal = color2Vec3(normal_color);
			real_normal = (new_x * real_normal.x + new_y * real_normal.y + new_z * real_normal.z).normalized();
			float intensity = std::max(real_normal * light_direction, 0.f);
			
			color.r *= (intensity * diffusion_coeff);
			color.g *= (intensity * diffusion_coeff);
			color.b *= (intensity * diffusion_coeff);
	
			return false;
		}
};

// const float diffusion_coef = 0.75;

int main(int argc, char** argv) {
	if (argc < 2){
		std::cerr << "Usage: " << argv[0] << " obj/model.obj" << std::endl;
        return 1;
	}
	
	// const char* name = nullptr;
	const char* specified_obj = nullptr;

	TGAImage image(width, height, TGAImage::RGB);
	
	float* zbuffer = new float[width*height];
	//set default values for z-buffer
	for (int i=width*height-1; i>=0; i--)
		zbuffer[i] = -std::numeric_limits<float>::max();

	Matrix projection = Matrix::identity(4);
	Matrix ViewPort = viewport(width/8, height/8, width*3/4, height*3/4, depth); // scale to the place where our eyes can see 
    projection[3][2] = -1.f/(camera - center).norm();

	Matrix model_view = move_camera(camera, center, vec3f(0, 1, 0));
	Matrix M = ViewPort * projection * model_view;

	for (int n = 1; n < argc; n++) {
		char path[128] = "texture/";
		specified_obj = argv[n];
		strcat(path, specified_obj);
		model = new Model(path);

		GouraudShader shader; 
		for (int i = 0; i < model->num_faces(); i++) {
			vec3f world[3];
			vec3i screen[3];
			vec2f uvs[3];
			std::vector<int> face = model->face(i);

			for(int j = 0; j < 3; j++) {
				world[j] = model->vert(face[3*j]);
				uvs[j] = model->uv(i, j);	
				
				vec3f normal_coord = model->norm(i, j);
				Matrix normal_coord_M = v2m(normal_coord); normal_coord_M[3][0] = 0; // keep it vector
				normal_coord_M = (projection * model_view).inverse_transpose() * normal_coord_M;

				normal_coord = vec3f(normal_coord_M[0][0], normal_coord_M[1][0],normal_coord_M[2][0] ); // convert it back to a vector
				
				shader.normals[j] = normal_coord;
				screen[j] = shader.vertex_shader(i, j, projection, model_view, ViewPort);
				
			}

			rasterize(screen, uvs[0], uvs[1], uvs[2], shader, zbuffer, model, image);
			
		}
		delete model;
	}
	image.flip_vertically(); 
	image.write_tga_file("output.tga");
	delete[] zbuffer;
}