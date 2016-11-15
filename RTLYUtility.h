#pragma once
#include <cmath>
#include "RTLYColor.h"
#include "RTLYConstant.h"

namespace RTLY{

    class World;
    class Material;
    class Vector3D;
    class Vector2D;
    typedef Vector3D Point3D;
    typedef Vector3D Normal;
    typedef Vector2D Point2D;

    class Matrix {
    public:
        double	m[4][4];
        Matrix(double diagonal=1.0);
        Matrix(const Matrix& mat);
        ~Matrix (void);
        Matrix& operator= (const Matrix& rhs); 	
        Matrix  operator* (const Matrix& mat) const;
        Matrix  operator* (const double d) const;
        void setIdentity(void);	
        //friend Vector3D operator*(const Matrix& mat,const Vector3D& v);
    };

    class Vector3D{
    public:
        double	x, y, z;
        Vector3D(double a=0.0):x(a),y(a),z(a){}
        Vector3D(double a, double b, double c):x(a),y(b),z(c){}
        Vector3D(const Vector3D& v):x(v.x),y(v.y),z(v.z){}

        ~Vector3D(void){}

        Vector3D& operator= (const Vector3D& rhs);
        Vector3D operator- (void) const{
            return Vector3D(-x,-y,-z);
        }	
        double length();
        double length2(){
            return x*x+y*y+z*z;
        }
        Vector3D operator/ (const double a) const{
            return Vector3D(x/a,y/a,z/a);
        }
        Vector3D operator+ (const Vector3D& v) const{
            return Vector3D(x+v.x,y+v.y,z+v.z);
        }
        Vector3D& operator+= (const Vector3D& v){
            x += v.x; y += v.y; z += v.z;
            return *this;
        }
        Vector3D operator- (const Vector3D& v) const{
            return Vector3D(x-v.x,y-v.y,z-v.z);
        }
        double operator* (const Vector3D& v) const{
            return x*v.x+y*v.y+z*v.z;
        }
        Vector3D operator^ (const Vector3D& v) const{
            return Vector3D(y*v.z-z*v.y,z*v.x-x*v.z,x*v.y-y*v.x);
        }
        void normalize(); 
        Vector3D& hat();
        double distance(const Vector3D& v) const;
        double distance2(const Vector3D& v) const;
        friend Vector3D operator*(Vector3D v,double a) {
            return Vector3D(v.x*a,v.y*a,v.z*a); 
        }

        friend Vector3D operator*(double a,Vector3D v){
            return Vector3D(v.x*a,v.y*a,v.z*a);
        }
        //friend Vector3D operator*(const Matrix& mat,const Vector3D& v);
    };

    class Vector2D{
    public:
        double	x, y;
        Vector2D(double a=0.0):x(a),y(a){}
        Vector2D(double a, double b):x(a),y(b){}
        Vector2D(const Vector2D& v):x(v.x),y(v.y){}

        ~Vector2D(void){}

        Vector2D& operator= (const Vector2D& rhs);
        Vector2D operator- (void) const{
            return Vector2D(-x,-y);
        }	
        double length();
        double length2(){
            return x*x+y*y;
        }
        Vector2D operator/ (const double a) const{
            return Vector2D(x/a,y/a);
        }
        Vector2D operator+ (const Vector2D& v) const{
            return Vector2D(x+v.x,y+v.y);
        }
        Vector2D& operator+= (const Vector2D& v){
            x += v.x; y += v.y;
            return *this;
        }
        Vector2D operator- (const Vector2D& v) const{
            return Vector2D(x-v.x,y-v.y);
        }
        double operator* (const Vector2D& v) const{
            return x*v.x+y*v.y;
        }
        void normalize(); 
        Vector2D& hat();
        friend Vector2D operator*(Vector2D v,double a) {
            return Vector2D(v.x*a,v.y*a); 
        }

        friend Vector2D operator*(double a,Vector2D v){
            return Vector2D(v.x*a,v.y*a);
        }
    };
   

    class Ray{
    public:
        Point3D o;
        Vector3D d;
        Ray():o(0.0),d(0.0,0.0,1.0){}
        Ray(const Point3D &p,const Vector3D &v):o(p),d(v){}
        Ray(const Ray& ray):o(ray.o),d(ray.d){}
        ~Ray(){}

        Ray& operator=(const Ray& rhs);
    };
    class ShadeRec{
    public:
        bool hitAnObject;
        Material* material;
        Point3D hitPoint;
        Point3D localHitPoint;
        Normal normal;
        RGBColor color;
        Ray ray;
        int depth;
        Vector3D dir;
        World& w;
        double t;
        ShadeRec(World &wr):
            hitAnObject(false),material(NULL),hitPoint(),localHitPoint(),
            normal(),color(black),ray(),depth(0),dir(),w(wr){}
        ShadeRec(const ShadeRec& sr);
        ~ShadeRec(){}

    };

    class BBox{
        public:
            double x0, x1, y0, y1, z0, z1;
            BBox(void);										
            BBox(const double x0, const double x1,			
                 const double y0, const double y1, 
                 const double z0, const double z1);
            BBox(const Point3D p0, const Point3D p1);											
            BBox(const BBox& bbox);							
            BBox& operator= (const BBox& rhs);			
            ~BBox(void);										
            bool hit(const Ray& ray) const;
            bool inside(const Point3D& p) const;
    };
}