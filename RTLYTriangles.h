#pragma once
#include "RTLYGeoObjects.h"

namespace RTLY{
    class Triangle : public GeoObject{
    public:
        Triangle(void);   												
        Triangle(const Point3D& a, const Point3D& b, const Point3D& c);
        virtual Triangle* clone(void) const;
        Triangle(const Triangle& triangle); 							
        ~Triangle(void);   											
        Triangle& operator= (const Triangle& rhs);
        virtual BBox get_bounding_box(void);
        void compute_normal(void);
        virtual bool hit(const Ray& ray, double& tmin, ShadeRec& sr) const;		 					 
        virtual bool shadow_hit(const Ray& ray, double& tmin) const; 
    private:
        Point3D	v0, v1, v2;
        Normal	normal;
    };
}