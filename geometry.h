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
        Vec2<t> (const Vec2<t>& v): x(v.x), y(v.y){ *this = v;}
        template <class u> 
        Vec2<t>(const Vec2<u> & v);
        inline Vec2<t> operator+ (const Vec2<t>& a){
            return Vec2<t>(x+a.x, y+a.y);
        }
        inline Vec2<t> operator- (const Vec2<t>& a){
            return Vec2<t>(x-a.x, y-a.y);
        }
        inline Vec2<t> operator* (float a){  // scalar product
            return Vec2<t>(x*a, a*y);
        }

        float distance (Vec2<t>& v){
            float dist = sqrt(pow(v.x - x, 2) + pow(v.y -y, 2));
            return dist;
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

template <class t>
class Vec3{
    
    public:
        t x, y, z;
        Vec3():x(0), y(0), z(0){}
        Vec3(t x, t y, t z):x(x), y(y), z(z){}
        Vec3<t> (const Vec3<t>& v): x(v.x), y(v.y), z(v.z){ *this = v;}
        // The case of same types for parameter and return type, we use above one
        // because compiler prefer the one with fixed type.
        template <class u> 
        Vec3<t>(const Vec3<u> & v);
        t X(){
            return x;
        }
        t Y(){
            return y;
        }
        t Z(){
            return z;
        }

        inline Vec3<t> operator+ (const Vec3<t>& a){
            return Vec3<t>(x+a.x, y+a.y, z+a.z);
        }
        
        inline Vec3<t> operator- (const Vec3<t>& a){
            return Vec3<t>(x-a.x, y-a.y, z-a.z);
        }
        inline Vec3<t> operator* (t a){  // scalar product
            return Vec3<t>(x*a, a*y, a*z);
        }
        inline t operator* (Vec3<t>& a){ // dot product
            return x*a.x + y*a.y + z*a.z;
        }
        inline Vec3<t> operator^ (const Vec3<t>& a){ //cross product
            return Vec3<t>(y*a.z - z*a.y, z*a.x-x*a.z, x*a.y-y*a.x);
        }
        inline float operator[] (int a) {
            if (a == 0)
                return x;
            if (a == 1)
                return y;
            if (a == 2)
                return z;
            return 0;
        }
        float norm (){
            return std::sqrt(x*x+y*y+z*z);
        }
        Vec3<t>& normalized(){
            *this = *this * (1/norm());
            return *this;
        }        
        float distance (Vec3<t>& v){
            float dist = sqrt(pow(v.x - x, 2) + pow(v.y - y, 2) + pow(v.z - z, 2));
            return dist;
        }
        friend std::ostream& operator<< (std::ostream& os, Vec3<t> v){
            os<<"("<<v.x <<", "<<v.y<<", "<<v.z<<")\n";
            return os;
        }
};
typedef Vec3<float> vec3f;
typedef Vec3<int> vec3i;

/////////////////////////////////////////////////////////////////////////////
const int DEFAULT_ALLOC = 4;
class Matrix{
    private:
        int rows, cols;

    public:
        std::vector<std::vector<float>> m;
        Matrix(int r=DEFAULT_ALLOC, int c = DEFAULT_ALLOC);
        
        inline int nrows(){return rows;}
        inline int ncols(){return cols;}

        static Matrix identity(int dimensions);
        std::vector<float>& operator[] (const int i);
        Matrix operator* (const Matrix& a);
        Matrix transpose();
        Matrix inverse();

        friend std::ostream& operator<<(std::ostream& s, Matrix& m);
};
#endif