#include <cstring>
#include <filesystem>
#include "rendering.h"
const float eps = 1e-5f;
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

const int width = 2000;
const int height = 2000;
const int depth = 255;

const double diffusion_coeff =  1.3;

float* shadow_buffer = new float[width * height];
float* frame_buffer = new float[width*height];

vec3f camera(0.25, 0.25, 0.75); // set our camera
vec3f center(0, 0, -2);
vec3f light_direction = vec3f(0,0,1).normalized(); // need to convert that into world space

Model* model = nullptr;

class DepthShader : public Shader{
	public:
		vec3f vertex_shader(int iface, int nthvert, Matrix projection, Matrix model_view, Matrix ViewPort){
			std::vector<int> face = model->face(iface);
			vec3f world = model->vert(face[3*nthvert]);
			Matrix M = ViewPort * projection * model_view;
			
			ndc_coord[nthvert] = m2v(M*v2m(world, true));


			// return vec3f(tmp[0][0], tmp[1][0], tmp[2][0]);
			return ndc_coord[nthvert];
		}

		bool fragment(vec2f uvP, vec3f nmA, vec3f nmB, float* zbuffer, vec3f P, int idx, float phi,  TGAColor& color, vec3i screen[3], vec2f uv0, vec2f uv1, vec2f uv2){
			if (zbuffer[idx] >= P.z)	
					return true;
			
			// color = white;
			color.r = white.r * (static_cast<float>(P.z) / static_cast<float>(depth));
			color.g = white.g * (static_cast<float>(P.z) / static_cast<float>(depth));
			color.b = white.b * (static_cast<float>(P.z) / static_cast<float>(depth));
			color.a = white.a * (static_cast<float>(P.z) / static_cast<float>(depth));
			
			return false;
		}
};

class GouraudShader : public Shader{
	public:
		Matrix fragment_2_shadow{4,4}; 
		
		GouraudShader(Matrix MS): fragment_2_shadow(MS){}

		vec3f vertex_shader(int iface, int nthvert, Matrix projection, Matrix model_view, Matrix ViewPort){

			std::vector<int> face = model->face(iface);
			vec3f world = model->vert(face[3*nthvert]);
			
			Matrix M = ViewPort * projection * model_view;
			// light_direction = m2v(M * v2m_vt(light_direction)).normalized();
			vec3f screen = m2v(M * v2m(world, true)); // we can divide that w at the end because viewport does not affect w
			ndc_coord[nthvert] = m2v(projection * model_view * v2m(world, true));
			return screen;
		}

		bool fragment(vec2f uvP, vec3f nmA, vec3f nmB, float* zbuffer, vec3f P, int idx, float phi,  TGAColor& color, vec3i screen[3], vec2f uv0, vec2f uv1, vec2f uv2){	
			if (zbuffer[idx] >= P.z)	
				return true;
			
			Matrix light_direction_new_basis = M_light * v2m(light_direction, false);
			vec3f l (light_direction_new_basis[0][0], light_direction_new_basis[1][0], light_direction_new_basis[2][0]);
			Matrix shadow_buffer_points = v2m(P, true);
			shadow_buffer_points =  fragment_2_shadow * shadow_buffer_points; // get the points in shadow mapping coord

			vec3f sb_pts = m2v(shadow_buffer_points);
			int idx_shadow = int(sb_pts[0]) + int(sb_pts[1]) * width;
			
			float shadow = 0.7f + 0.5f * (shadow_buffer[idx_shadow] - sb_pts[2] < eps);

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
			real_normal = Darboux2World(new_x, new_y, new_z, real_normal);

			vec3f reflection = (2.0f * (real_normal * l * real_normal) - l).normalized();
			float diff = std::max(real_normal * l, 0.f);
			
			float spec = std::pow(std::max(reflection.z, 0.f), std::max(float(model->specular(uvP).b), 30.f));

			color.r = (unsigned char)std::min(0.5 + color.r * (diff * diffusion_coeff) * shadow  + color.r * 0.6 * spec * shadow , 255.0);
			color.g = (unsigned char) std::min(0.5 + color.g * (diff * diffusion_coeff) * shadow + color.g * 0.6 * spec * shadow, 255.0);
			color.b = (unsigned char) std::min(0.5 + color.b * (diff * diffusion_coeff) * shadow + color.b * 0.6 * spec * shadow, 255.0);
			color.a = (unsigned char) std::min(0.5 + color.a * (diff * diffusion_coeff) * shadow + color.a * 0.6 * spec * shadow, 255.0);

	
			return false;
		}
};

// const float diffusion_coef = 0.75;

int main(int argc, char** argv) {
	if (argc < 2){
		std::cerr << "Usage: " << argv[0] << " obj/model.obj" << std::endl;
        return 1;
	}

	const char* specified_obj = nullptr;

	
	//set default values for z-buffer
	for (int i=width*height-1; i>=0; i--){
		frame_buffer[i] = std::numeric_limits<float>::min();
		shadow_buffer[i] = std::numeric_limits<float>::min();
	}

	Matrix ViewPort = viewport(width/8, height/8, width*3/4, height*3/4, depth);

	//Following scope is for the shadow map
	{
		TGAImage image(width, height, TGAImage::RGB);
		Matrix projection = Matrix::identity(4); // we do not need the perspective view for the shadow buffering
		projection[3][2] = 0;
		Matrix model_view = move_camera(light_direction, center, vec3f(0, 1, 0));
		Matrix M = ViewPort * projection * model_view;

		for (int n = 1; n < argc; n++) {
			char path[128] = "texture/";
			specified_obj = argv[n];
			strcat(path, specified_obj);
			model = new Model(path);
			DepthShader shader;
			for (int i = 0; i < model->num_faces(); i++) {
				vec3f world[3];
				vec3i screen[3];
				vec2f uvs[3];
				std::vector<int> face = model->face(i);

				for(int j = 0; j < 3; j++) {
					world[j] = model->vert(face[3*j]);
					uvs[j] = model->uv(i, j);	
					screen[j] = shader.vertex_shader(i, j, projection, model_view, ViewPort);					
					
				}

				rasterize(screen, uvs[0], uvs[1], uvs[2], shader, shadow_buffer, model, image);
				
			}
			delete model;
		}
		image.flip_vertically(); 
		image.write_tga_file("depth.tga");
	}

	// Following scope is for the fragment buffer
	{	
		TGAImage image(width, height, TGAImage::RGB);
		Matrix projection = Matrix::identity(4);
		projection[3][2] = -1.f/(camera - center).norm();

		Matrix model_view = move_camera(camera, center, vec3f(0, 1, 0));
		Matrix M = ViewPort * projection * model_view;
		Matrix m_inverse = M.inverse();

		Matrix MS =  M * m_inverse;

		for (int n = 1; n < argc; n++) {
			char path[128] = "texture/";
			specified_obj = argv[n];
			strcat(path, specified_obj);
			model = new Model(path);

			GouraudShader shader(MS); 
			shader.M_light = projection * model_view;

			for (int i = 0; i < model->num_faces(); i++) {
				vec3f world[3];
				vec3i screen[3];
				vec2f uvs[3];
				std::vector<int> face = model->face(i);

				for(int j = 0; j < 3; j++) {
					world[j] = model->vert(face[3*j]);
					uvs[j] = model->uv(i, j);	
					
					vec3f normal_coord = model->norm(i, j);
					Matrix normal_coord_M = v2m(normal_coord, true); normal_coord_M[3][0] = 0; // keep it vector
					normal_coord_M = (projection * model_view).inverse_transpose() * normal_coord_M;

					normal_coord = vec3f(normal_coord_M[0][0], normal_coord_M[1][0],normal_coord_M[2][0] ); // convert it back to a vector
					
					shader.normals[j] = normal_coord;
					screen[j] = shader.vertex_shader(i, j, projection, model_view, ViewPort);
					
				}

				rasterize(screen, uvs[0], uvs[1], uvs[2], shader, frame_buffer, model, image);
				
			}
			delete model;
		}
		image.flip_vertically(); 
		image.write_tga_file("output.tga");
	}
	delete[] shadow_buffer;
	delete[] frame_buffer;
}