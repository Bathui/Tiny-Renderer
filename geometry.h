#ifndef GEOMETRY_H
#define GEOMETRY_H
#include <iostream>
#include <cmath>
template <typename t>
class Vec2{  // a 2d vector
        
    public:
        t x, y;
        Vec2 ():x(0), y(0){}
        Vec2 (t x, t y):x(x), y(y){};
        
        inline Vec2<t> operator+ (Vec2<t>& a){
            return Vec2<t>(x+a.x, y+a.y);
        }
        inline Vec2<t> operator- (Vec2<t>& a){
            return Vec2<t>(x-a.x, y-a.y);
        }
        inline Vec2<t> operator* (float a){  // scalar product
            return Vec2<t>(x*a, a*y);
        }
        inline t operator* (Vec2<t>& a){
            return x*a.x + y*a.y;
        } // dot product

        friend std::ostream& operator<< (std::ostream& os, Vec2<t> v){
            os<<"("<<v.x <<", "<<v.y<<")\n";
            return os;
        } 
};
typedef Vec2<float> vec2f;
typedef Vec2<int>   vec2i;

template <typename t>
class Vec3{
    
    public:
        t x, y, z;
        Vec3():x(0), y(0), z(0){}
        Vec3(t x, t y, t z):x(x), y(y), z(z){}

        t X(){
            return x;
        }
        t Y(){
            return y;
        }
        t Z(){
            return z;
        }

        inline Vec3<t> operator+ (Vec3<t>& a){
            return Vec3<t>(x+a.x, y+a.y, z+a.z);
        }
        
        inline Vec3<t> operator- (Vec3<t>& a){
            return Vec3<t>(x-a.x, y-a.y, z-a.z);
        }
        inline Vec3<t> operator* (t a){  // scalar product
            return Vec3<t>(x*a, a*y, a*z);
        }
        inline t operator* (Vec3<t>& a){ // dot product
            return x*a.x + y*a.y + z*a.z;
        }
        inline Vec3<t> operator^ (Vec3<t>&& a){ //cross product
            return Vec3<t>(y*a.z - z*a.y, z*a.x-x*a.z, x*a.y-y*a.x);
        }

        float norm (){
            return std::sqrt(x*x+y*y+z*z);
        }
        Vec3<t>& normalized(){
            *this = *this * 1/norm();
            return *this;
        }        
        friend std::ostream& operator<< (std::ostream& os, Vec3<t> v){
            os<<"("<<v.x <<", "<<v.y<<", "<<v.z<<")\n";
            return os;
        }
};
typedef Vec3<float> vec3f;
typedef Vec3<int> vec3i;

#endif