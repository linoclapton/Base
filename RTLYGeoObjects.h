#pragma once
#include "RTLYColor.h"
#include "RTLYUtility.h"

namespace RTLY{

    class Sampler;

class GeoObject{
protected:
    Material* material;
    RGBColor color;
public:
    virtual bool hit(const Ray& ray,double& tmin,ShadeRec& sr)const=0;
    RGBColor getColor(){return color;}
    void setMaterial(Material* m){material = m;}
    Material* getMaterial(){return material;}
    virtual Point3D sample(void){return Point3D(0);}
    virtual Normal getNormal(const Point3D& p){
        return Vector3D(0.0,1.0,0.0);
    }
    virtual float pdf(const ShadeRec& sr){return 1.0;}
    void setColor(float r,float g,float b){ color = RGBColor(r,g,b); }
    virtual bool shadowHit(const Ray& ray,float& tmin) const {return false;}
};

class Plane:public GeoObject{
private:
    Point3D point;
    Normal normal;
    static const double kEpsilon;
public:
    Plane();
    Plane(const Point3D &p,const Normal &n):point(p),normal(n){}
    virtual bool hit(const Ray& ray,double &t,ShadeRec& s) const;
    virtual bool shadowHit(const Ray& ray,float& tmin) const;
};

class Sphere:public GeoObject{
public:
    Point3D 	center;
    double 		radius;
    static const double kEpsilon;
public:
    Sphere():center(0.0),radius(1.0){}
    Sphere(Point3D center, double r):center(center),radius(r){}
    Sphere(const Sphere& sphere):center(sphere.center),radius(sphere.radius){}

    Sphere& operator=(const Sphere& rhs);

    virtual bool hit(const Ray& ray,double &t,ShadeRec& s) const;
    virtual bool shadowHit(const Ray& ray,float& tmin) const;
};

class Rectangle : public GeoObject{
public:
    Rectangle(void);   									
    Rectangle(const Point3D& p0, const Vector3D& a, const Vector3D& b);
    Rectangle(const Point3D& p0, const Vector3D& a, const Vector3D& b, const Normal& n);
    virtual Rectangle* clone(void) const;
    Rectangle(const Rectangle& r); 						
    virtual ~Rectangle(void);   									
    Rectangle& operator= (const Rectangle& rhs);

    virtual bool hit(const Ray& ray, double& t, ShadeRec& s) const;	
    virtual void setSampler(Sampler* sampler); 
    virtual Point3D sample(void);
    virtual Normal getNormal(const Point3D& p);		
    virtual float pdf(const ShadeRec& sr);

private:
    Point3D 		p0;   			// corner vertex 
    Vector3D		a;				// side
    Vector3D		b;				// side
    double			alen2;	        // square of the length of side a
    double			blen2;	        // square of the length of side b
    Normal			normal;	
    float			area;			// for rectangular lights
    float			invArea;		// for rectangular lights
    Sampler*		sampler;	    // for rectangular lights 	

    static const double kEpsilon;   											
};
class ConvexPartSphere: public GeoObject {	
public:
    Point3D 	center;   			// center coordinates 
    double 		radius;				// sphere radius 
    double 		phi_min;			// minimum azimiuth angle in degrees measured counter clockwise from the +ve z axis
    double 		phi_max;			// maximum azimiuth angle in degrees measured counter clockwise from the +ve z axis
    double 		theta_min;			// minimum polar angle in degrees measured down from the +ve y axis
    double 		theta_max;			// maximum polar angle in degrees measured down from the +ve y axis
    double		cos_theta_min;		// stored to avoid repeated calculations
    double		cos_theta_max;		// stored to avoid repeated calculations
public:
    ConvexPartSphere(void);   									    
    ConvexPartSphere(const Point3D 	c, const double 	r, 
        const double 	azimuth_min,	// in degrees
        const double 	azimuth_max,	// in degrees
        const double 	polar_min,		// in degrees measured from top
        const double 	polar_max);		// in degrees measured from top
    ConvexPartSphere(const Point3D c, const double radius);
    virtual ConvexPartSphere* clone(void) const;
    ConvexPartSphere(const ConvexPartSphere& cp); 						
    virtual ~ConvexPartSphere(void);   									
    ConvexPartSphere& operator= (const ConvexPartSphere& rhs);								
    virtual bool hit(const Ray& ray, double& tmin, ShadeRec& sr) const;	
};
}