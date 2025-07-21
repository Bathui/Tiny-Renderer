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

void rasterize(vec3f* pts, float* zbuffer, float intensity, TGAImage& img, vec2f* tex_coords, TGAImage& texture, Model& model){
    vec2f boxmin( width,  height);
    vec2f boxmax(0, 0);

    for(int i = 0; i < 3; i++){
        //Outer max is to handle the invalid cases
        boxmin.x = std::max(0.f, std::min(pts[i].x, boxmin.x));
        boxmin.y = std::max(0.f, std::min(pts[i].y, boxmin.y)); 
        
        boxmax.x = std::min(std::max(pts[i].x, boxmax.x), (float)img.get_width()-1);
        boxmax.y = std::min(std::max(pts[i].y, boxmax.y), (float)img.get_height()-1);
        
    }
    
    vec3f inter_points;
	
    for(inter_points.x = boxmin.x; inter_points.x<= boxmax.x; inter_points.x++) {
		for(inter_points.y = boxmin.y; inter_points.y<= boxmax.y; inter_points.y++){
            vec3f coefficient = baryCentric(pts, inter_points);
            if(coefficient.x < 0 ||coefficient.y < 0|| coefficient.z < 0)
                continue; //there are some points not in our simplex, so we need to skip those points
            inter_points.z = 0;
			for(int i = 0; i < 3; i++){
				if(i==0){
					inter_points.z += (pts[i].z* coefficient.x);

				}
				else if(i==1){
					inter_points.z += (pts[i].z* coefficient.y);
					
				}
				else if(i==2){
					inter_points.z += (pts[i].z* coefficient.z);
					
				}
				else;	
			}
			if (zbuffer[int(inter_points.x + inter_points.y * width)] < inter_points.z) {
				zbuffer[int(inter_points.x + inter_points.y * width)] = inter_points.z;
				
				vec2f uv = vec2f(
					tex_coords[0].x * coefficient.x + tex_coords[1].x * coefficient.y + tex_coords[2].x * coefficient.z,
					tex_coords[0].y * coefficient.x + tex_coords[1].y * coefficient.y + tex_coords[2].y * coefficient.z
				);

				TGAColor color = model.diffuse(uv);
				// TGAColor color = white;
				color.a = 255;
				color.r *= intensity;
				color.g *= intensity;
				color.b *= intensity;
				
				
				img.set(inter_points.x, inter_points.y, color);//should fix
			}
        }
    }

}