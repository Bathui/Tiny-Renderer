#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0,   255, 0,   255);
const int width  = 200;
const int height = 200;

void line(vec2i p0, vec2i p1, TGAImage &image, TGAColor color) {
    bool steep = false;
    if (std::abs(p0.x-p1.x)<std::abs(p0.y-p1.y)) {
        std::swap(p0.x, p0.y);
        std::swap(p1.x, p1.y);
        steep = true;
    }
    if (p0.x>p1.x) {
        std::swap(p0, p1);
    }

    for (int x=p0.x; x<=p1.x; x++) {
        float t = (x-p0.x)/(float)(p1.x-p0.x);
        int y = p0.y*(1.-t) + p1.y*t;
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
    }
}

void triangle(vec2i t0, vec2i t1, vec2i t2, TGAImage &image, TGAColor color){
    if (t0.y == t1.y && t1.y== t2.y)//three vertices on the same line
        return ;
    //sort the vertices
    if (t0.y > t1.y)
        std::swap(t0, t1);
    if (t0.y > t2.y)
        std::swap(t0, t2);
    if (t1.y > t2.y)
        std::swap(t1, t2);
    
    int tot_height = t2.y - t0.y; // max height for the triangle
    for (int i = 0 ; i < tot_height; i++) {
        bool second_half = t0.y == t1.y || i > t1.y- t0.y;
        int seg_height = second_half ? t2.y - t1.y : t1.y - t0.y;

        float alpha = (float)i/tot_height;
        float beta = (float)(i-(second_half ? t1.y-t0.y:0))/seg_height;
        vec2i A = (t2-t0)*alpha + t0;
        vec2i B = second_half ? (t2-t1)*beta + t1 : (t1-t0)*beta + t0; 
        
        if (A.x > B.x)
            std::swap(A, B);
        for (int j = A.x; j<=B.x; j++) {
            image.set(j, t0.y+i, color);
        }  
    }
    int seg_height = t2.y - t1.y;
    float alpha = 1.0;
    float beta = (float)(tot_height-(t1.y-t0.y))/seg_height;
    vec2i A = (t2-t0)*alpha + t0;
    vec2i B = (t2-t1)*beta + t1;
    if (A.x > B.x)
            std::swap(A, B);
    for (int j = A.x; j<=B.x; j++) {
        image.set(j, t0.y+tot_height, color);
    }

}
vec3f baryCentric(vec2f* vertices, vec2f p){
    vec3f coefficient = vec3f(vertices[2].x - vertices[0].x, vertices[1].x - vertices[0].x, vertices[0].x - p.x) ^ vec3f(vertices[2].y - vertices[0].y, vertices[1].y - vertices[0].y, vertices[0].y - p.y);
    
    if(abs(coefficient.z) < 1)
        return vec3f(-1, -1, -1); // it means the triangle is trivial
    
    return vec3f(coefficient.x/coefficient.z, coefficient.y/coefficient.z, 1); // return the barycentric coefficient
}
int main(int argc, char** argv) {
    TGAImage image(width, height, TGAImage::RGB);

    vec2i t0[3] = {vec2i(0, 0),   vec2i(50, 160),  vec2i(70, 80)};
    vec2i t1[3] = {vec2i(180, 50),  vec2i(150, 1),   vec2i(70, 180)};
    vec2i t2[3] = {vec2i(180, 150), vec2i(120, 160), vec2i(130, 180)};

    triangle(t0[0], t0[1], t0[2], image, red);
    triangle(t1[0], t1[1], t1[2], image, white);
    triangle(t2[0], t2[1], t2[2], image, green);


    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    return 0;
}