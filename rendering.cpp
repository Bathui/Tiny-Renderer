#include "rendering.h"
const int width = 2000;
const int height = 2000;
const TGAColor white   = {255, 255, 255, 255}; 

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

void rasterize(vec3i t0, vec3i t1, vec3i t2, vec2f uv0, vec2f uv1, vec2f uv2, float intensity, float *zbuffer, Model* model, TGAImage& img){
	if (t0.y == t1.y && t0.y == t2.y)
		return;
	
	if (t0.y > t1.y) {
		std::swap(t0, t1);
		std::swap(uv0, uv1);
	}
	if (t0.y > t2.y) {
		std::swap(t0, t2);
		std::swap(uv0, uv2);
	}
	if (t1.y > t2.y) {
		std::swap(t1, t2);
		std::swap(uv1, uv2);
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
		if (A.x > B.x) {
			std::swap(A, B);
			std::swap(uvA, uvB);
		}

		for (int j = A.x; j <= B.x; j++) { // start horizontal scanline interpolation
			float phi = B.x==A.x ? 1.f : (float)(j-A.x)/(float)(B.x - A.x);
			
			vec3i P = vec3f(A) + vec3f(B-A) * phi;
			vec2f uvP = uvA + vec2f(uvB - uvA) * phi;

			int idx = P.x + P.y * width;
			if (zbuffer[idx] < P.z) {
				zbuffer[idx] = P.z;
				TGAColor color = model->diffuse(uvP);
				// TGAColor color = white;

				color.a = 255;
				color.r *= intensity;
				color.g *= intensity;
				color.b *= intensity;

				img.set(P.x, P.y, color);
			}
		}
	}

}