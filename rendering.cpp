#include "rendering.h"
const int width = 2000;
const int height = 2000;
const TGAColor white   = {255, 255, 255, 255}; 
const double diffusion_coeff =  1.3;
// Shader::~Shader(){}
/** 
 * Shader contains the new normal mapping information
 * Create a new container for the normal mapping in our Model object
*/

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) { 
    bool steep = false;
	if (std::abs(x0-x1) < std::abs(y0-y1)) {
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}

	if (x1 < x0) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	int dx = x1 - x0;
	int dy = y1 - y0;
	int derror2 = std::abs(dy)*2;
	int error = 0;
	int y = y0;
	for (int x = x0; x <= x1; x++) { 
		if (steep){
			image.set(y, x, color);
		}
		else{
			image.set(x, y, color);
		}
		error += derror2;
		if (error> dx){
			y += (y1>y0? 1:-1);
			error -= 2*dx;
		}		
    } 
}
vec3f baryCentric(vec3f* vertices, vec3f p){
    vec3f coefficient = vec3f(vertices[2].x - vertices[0].x, vertices[1].x - vertices[0].x, vertices[0].x - p.x) ^ vec3f(vertices[2].y - vertices[0].y, vertices[1].y - vertices[0].y, vertices[0].y - p.y);
    
    if(std::abs(coefficient.z) < 1)
        return vec3f(-1, 1, 1); // it means the triangle is trivial
    
    return vec3f(1.f-(coefficient.x+coefficient.y)/coefficient.z, coefficient.y/coefficient.z, coefficient.x/coefficient.z); // return the barycentric coefficient
}

vec3f m2v(Matrix m){
	return vec3f(m[0][0]/m[3][0], m[1][0]/m[3][0], m[2][0]/m[3][0]);
}

Matrix v2m (vec3f& v) {
	Matrix m(4, 1);
	
	m[0][0] = v.x;
	m[1][0] = v.y;
	m[2][0] = v.z;

	m[3][0] = 1;

	return m;
}

Matrix viewport(int x, int y, int w, int h, int d){
	Matrix m = Matrix::identity(4);
	
	m[0][0] = w/2.f;
	m[1][1] = h/2.f;
	m[2][2] = d/2.f;

	m[0][3] = x + w/2.f;
	m[1][3] = y + h/2.f;
	m[2][3] = d/2.f;

	return m;	
}

Matrix move_camera(vec3f eye, vec3f center, vec3f up){
	vec3f z = (eye - center).normalized();
	vec3f x = (up ^ z).normalized();
	vec3f y = up.normalized(); // if that does not work, change it afterwards

	Matrix Minv = Matrix::identity(4);
	Matrix Tr = Matrix::identity(4);

	for (int i = 0; i < 3; i++) {
		Minv[0][i] = x[i];
		Minv[1][i] = y[i];
		Minv[2][i] = z[i];
		
		Tr[i][3] = -eye[i]; 
	}

	Matrix Model_View = Minv * Tr;
	return Model_View;
}

vec3f new_basis(Matrix& Ainv, float f0, float f1, float f2){
	Matrix B (3, 1);
	B[0][0] = f1-f0; B[1][0] = f2 - f0; B[2][0] = 0;
	Matrix res = Ainv * B;
	return vec3f(res[0][0], res[1][0], res[2][0]).normalized();
}

vec3f color2Vec3(TGAColor& color){
	return vec3f(color.r/255.f * 2.f - 1.f, 
				color.g/255.f * 2.f - 1.f, 
				color.b/255.f * 2.f - 1.f);  // we extract the rgb value as the diverted result
}

vec3f Darboux2World(vec3f new_x, vec3f new_y, vec3f new_z, vec3f normal_coord){
	vec3f world = new_x * normal_coord.x + new_y * normal_coord.y + new_z * normal_coord.z;
	return world.normalized();
}

void rasterize(vec3i screen[3], vec2f uv0, vec2f uv1, vec2f uv2, Shader& shader, float *zbuffer, Model* model, TGAImage& img){
	vec3i& t0 = screen[0]; // here we got the problem ~
	vec3i& t1 = screen[1];
	vec3i& t2 = screen[2];
	

	if (t0.y == t1.y && t0.y == t2.y)
		return;
	
	if (t0.y > t1.y) {
		std::swap(t0, t1);
		std::swap(uv0, uv1);
		std::swap(shader.ndc_coord[0], shader.ndc_coord[1]);
		std::swap(shader.normals[0], shader.normals[1]);
		// std::swap(it0, it1);
	}
	if (t0.y > t2.y) {
		std::swap(t0, t2);
		std::swap(uv0, uv2);
		std::swap(shader.ndc_coord[0], shader.ndc_coord[2]);
		std::swap(shader.normals[0], shader.normals[2]);
		// std::swap(it0, it2);
	}
	if (t1.y > t2.y) {
		std::swap(t1, t2);
		std::swap(uv1, uv2);
		std::swap(shader.ndc_coord[1], shader.ndc_coord[2]);
		std::swap(shader.normals[1], shader.normals[2]);
		// std::swap(it1, it2);
	}

	int max_height = t2.y - t0.y;

	for (int i = 0; i < max_height; i++) { // vertical scanline interpolation
		bool second_half = i > t1.y-t0.y || t1.y == t0.y;
		int segment_height = second_half ? t2.y - t1.y : t1.y-t0.y;
		
		float alpha = (float)i/max_height;
		float beta = (float)(i-(second_half ? t1.y-t0.y : 0))/segment_height; ;

		vec3i A = t0 + vec3f(t2-t0) * alpha;
		vec3i B = second_half ? t1 + vec3f(t2-t1) * beta : t0 +vec3f(t1-t0) * beta;

		vec2f uvA = uv0 + vec2f(uv2-uv0) * alpha;
		vec2f uvB = second_half ? uv1 + vec2f(uv2-uv1)* beta : uv0 + vec2f(uv1 - uv0) * beta;

		// float iA = it0 + (it2 - it0) * alpha;
		// float iB = second_half ? it1 + (it2 - it1) * beta : it0 + (it1 - it0) * beta;

		vec3f nmA = shader.normals[0] + vec3f(shader.normals[2]-shader.normals[0]) * alpha;
		vec3f nmB = second_half ? shader.normals[1] + vec3f(shader.normals[2]-shader.normals[1]) * beta : shader.normals[0] +vec3f(shader.normals[1]-shader.normals[0]) * beta;


		if (A.x > B.x) {
			std::swap(A, B);
			std::swap(uvA, uvB);
			// std::swap(iA, iB);
			std::swap(nmA, nmB);
		}

		for (int j = A.x; j <= B.x; j++) { // start horizontal scanline interpolation
			float phi = B.x==A.x ? 1.f : (float)(j-A.x)/(float)(B.x - A.x);
			
			vec3i P = vec3f(A) + vec3f(B-A) * phi;
			vec2f uvP = uvA + vec2f(uvB - uvA) * phi;
			// float ivP = iA + (iB - iA) * phi;
			TGAColor color = model->diffuse(uvP);

			int idx = P.x + P.y * width;
			bool discard = shader.fragment(uvP, nmA, nmB, zbuffer, P, idx, phi, color, screen, uv0, uv1, uv2); //need further changes here
			// std::cout<<discard<<std::endl;
			if (!discard) {
				zbuffer[idx] = P.z;
			    // std::cout<<(int)color.r<<" "<<(int)color.g<<" "<<(int)color.b<<std::endl;

				img.set(P.x, P.y, color);
			}
		}
	}

}