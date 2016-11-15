#include "RTLYGeoObjects.h"
#include "RTLYSampler.h"

using namespace RTLY;
double const Sphere::kEpsilon = kEpsilon;
double const Plane::kEpsilon = kEpsilon;
const double Rectangle::kEpsilon = kEpsilon;

bool Plane::hit(const Ray& ray,double &tmin,ShadeRec& s) const{

    double t = (point - ray.o)*normal/(ray.d*normal);
    if( t > kEpsilon ){
        tmin = t;
        s.normal = normal;
        s.normal.normalize();
        s.localHitPoint = ray.o + ray.d*t;
        return true;
    }else
        return false;
}

bool Plane::shadowHit(const Ray& ray,float& tmin) const{
    float t = (point - ray.o) * normal / (ray.d * normal);
    if (t > kEpsilon) {
        tmin = t;
        return (true);	
    }
    else
        return (false);
}

//------------------------------------------------------------------------------------------------------------------

Sphere& Sphere::operator=(const Sphere& rhs){
    if( this == &rhs )
        return *this;
    center = rhs.center; radius = rhs.radius;
    return *this;
}

bool Sphere::hit(const Ray& ray,double &tmin,ShadeRec& s) const{
    double t;
    Vector3D temp = ray.o - center;
    double a = ray.d * ray.d;
    double b = temp*ray.d*2.0;
    double c = temp*temp - radius*radius;
    double disc = b*b - 4*a*c;
    if(disc<0.0)
        return false;
    else{
        double e = sqrt(disc);
        double denom = 2.0*a;
        t = (-b - e)/denom;
        if( t>kEpsilon ){
            tmin = t;
            s.normal = (temp + ray.d*t);
            s.normal.normalize();
            s.localHitPoint = ray.o + ray.d*t;
            return true;
        }else{
            return false;
        }
    }
}

bool Sphere::shadowHit(const Ray& ray,float& tmin) const{
    double t;
    Vector3D temp = ray.o - center;
    double a = ray.d * ray.d;
    double b = temp*ray.d*2.0;
    double c = temp*temp - radius*radius;
    double disc = b*b - 4*a*c;
    if(disc<0.0)
        return false;
    else{
        double e = sqrt(disc);
        double denom = 2.0*a;
        t = (-b - e)/denom;
        if( t>kEpsilon ){
            tmin = t;
            return true;
        }else{
            return false;
        }
    };
}

//------------------------------------------------------------------------------------------------------------------


Rectangle::Rectangle(void)
    : 	GeoObject(),
    p0(-1, 0, -1), 
    a(0, 0, 2), b(2, 0, 0), 
    alen2(4.0), 
    blen2(4.0),
    normal(0, 1, 0),
    area(4.0),
    invArea(0.25),
    sampler(NULL)
{}

Rectangle::Rectangle(const Point3D& _p0, const Vector3D& _a, const Vector3D& _b)
    :	GeoObject(),
    p0(_p0),
    a(_a),
    b(_b),
    alen2(a.length2()), 
    blen2(b.length2()),
    area(a.length() * b.length()),
    invArea(1.0 / area),
    sampler(NULL)		
{
    normal = a ^ b;
    normal.normalize();
}

Rectangle::Rectangle(const Point3D& _p0, const Vector3D& _a, const Vector3D& _b, const Normal& n)
    :	GeoObject(),
    p0(_p0),
    a(_a),
    b(_b),
    alen2(a.length2()), 
    blen2(b.length2()),
    area(a.length() * b.length()),	
    invArea(1.0 / area),
    normal(n),
    sampler(NULL)
{
    normal.normalize();
}

Rectangle* 
    Rectangle::clone(void) const {
        return (new Rectangle(*this));
}

Rectangle::Rectangle (const Rectangle& r)
    :	GeoObject(r),
    p0(r.p0), 
    a(r.a),
    b(r.b),
    alen2(r.alen2), 
    blen2(r.blen2),	
    normal(r.normal),
    area(r.area),
    invArea(r.invArea)
{
    if(r.sampler)
        sampler	= r.sampler->clone(); 
    else  
        sampler = NULL;
}

Rectangle& Rectangle::operator= (const Rectangle& rhs) {
        if (this == &rhs)
            return (*this);

        GeoObject::operator=(rhs);

        p0				= rhs.p0;
        a				= rhs.a;
        b				= rhs.b;
        alen2	        = rhs.alen2; 
        blen2	        = rhs.blen2;
        area			= rhs.area;	
        invArea		= rhs.invArea;
        normal			= rhs.normal;

        if (sampler) {
            delete sampler;
            sampler = NULL;
        }

        if (rhs.sampler)
            sampler= rhs.sampler->clone();

        return (*this);
}

Rectangle::~Rectangle (void) {

    if (sampler) {
        delete sampler;
        sampler = NULL;
    }
}

bool Rectangle::hit(const Ray& ray, double& tmin, ShadeRec& sr) const {	

        double t = (p0 - ray.o) * normal / (ray.d * normal); 

        if (t <= kEpsilon)
            return (false);

        Point3D p = ray.o + t * ray.d;
        Vector3D d = p - p0;

        double ddota = d * a;

        if (ddota < 0.0 || ddota > alen2)
            return (false);

        double ddotb = d * b;

        if (ddotb < 0.0 || ddotb > blen2)
            return (false);

        tmin 				= t;
        sr.normal 			= normal;
        sr.localHitPoint 	= p;

        return (true);
}

void Rectangle::setSampler(Sampler* sampler) {
        this->sampler = sampler;
}

Point3D Rectangle::sample(void) {
        Point2D sample_point = sampler->sampleUnitSquare();
        return (p0 + sample_point.x * a + sample_point.y * b);
}

Normal Rectangle::getNormal(const Point3D& p) {
        return (normal);
}

float Rectangle::pdf(const ShadeRec& sr) {	
        return (invArea);
} 

//------------------------------------------------------------------------------------------------------------------

ConvexPartSphere::ConvexPartSphere (void)	
    : 	GeoObject(),
    center(0.0),
    radius(1.0),
    phi_min(0.0),			// in radians
    phi_max(TWO_PI),		// in radians	
    theta_min(0.0),			// in radians measured from top
    theta_max(PI),			// in radians measured from top
    cos_theta_min(1.0),
    cos_theta_max(-1.0)
{}


// ---------------------------------------------------------------- donstructor

ConvexPartSphere::ConvexPartSphere (const Point3D 	c, 
                                    const double 	r, 
                                    const double 	azimuth_min,	// in degrees
                                    const double 	azimuth_max,	// in degrees
                                    const double 	polar_min,		// in degrees measured from top
                                    const double 	polar_max)		// in degrees measured from top
                                    : 	GeoObject(),
                                    center(c),
                                    radius(r),
                                    phi_min(azimuth_min * PI_ON_180),			// in radians
                                    phi_max(azimuth_max * PI_ON_180),			// in radians
                                    theta_min(polar_min * PI_ON_180),			// in radians measured from top
                                    theta_max(polar_max * PI_ON_180),			// in radians measured from top
                                    cos_theta_min(cos(theta_min)),
                                    cos_theta_max(cos(theta_max))
{}

ConvexPartSphere::ConvexPartSphere (const Point3D 	c, 
                                    const double 	r)
                                    : 	GeoObject(),
                                    center(c),
                                    radius(r),
                                    phi_min(0.0),
                                    phi_max(TWO_PI),
                                    theta_min(0.0),
                                    theta_max(PI),
                                    cos_theta_min(1.0),
                                    cos_theta_max(-1.0)
{}

ConvexPartSphere* ConvexPartSphere::clone(void) const {
        return (new ConvexPartSphere(*this));
}

ConvexPartSphere::ConvexPartSphere (const ConvexPartSphere& ps)
    : 	GeoObject(ps),
    center(ps.center),
    radius(ps.radius),
    phi_min(ps.phi_min),
    phi_max(ps.phi_max),
    theta_min(ps.theta_min),
    theta_max(ps.theta_max),
    cos_theta_min(ps.cos_theta_min),
    cos_theta_max(ps.cos_theta_max) 
{}

ConvexPartSphere& ConvexPartSphere::operator= (const ConvexPartSphere& rhs)		
{
    if (this == &rhs)
        return (*this);

    GeoObject::operator=(rhs);

    center 			= rhs.center;
    radius			= rhs.radius;
    phi_min			= rhs.phi_min;
    phi_max			= rhs.phi_max;
    theta_min		= rhs.theta_min;
    theta_max		= rhs.theta_max;
    cos_theta_min	= rhs.cos_theta_min;
    cos_theta_max	= rhs.cos_theta_max; 

    return (*this);
}

ConvexPartSphere::~ConvexPartSphere(void) {}

bool ConvexPartSphere::hit(const Ray& ray, double& tmin, ShadeRec& sr) const {
        double 		t;
        Vector3D  	temp 	= ray.o - center;
        double 		a 		= ray.d * ray.d;
        double 		b 		= 2.0 * temp * ray.d;
        double 		c 		= temp * temp - radius * radius;
        double 		disc 	= b * b - 4.0 * a * c;

        if (disc < 0.0)
            return(false);
        else {	
            double e = sqrt(disc);
            double denom = 2.0 * a;
            t = (-b - e) / denom;    // smaller root

            if (t > kEpsilon) {
                Vector3D hit = ray.o + t * ray.d - center;

                double phi = atan2(hit.x, hit.z);
                if (phi < 0.0)
                    phi += TWO_PI;

                if (hit.y <= radius * cos_theta_min && 	
                    hit.y >= radius * cos_theta_max && 	
                    phi >= phi_min && phi <= phi_max) {

                        tmin = t;				
                        sr.normal = (temp + t * ray.d) / radius;   // points outwards
                        sr.localHitPoint = ray.o + tmin * ray.d;				
                        return (true);
                }
            } 

            t = (-b + e) / denom;    // larger root

            if (t > kEpsilon) {
                Vector3D hit = ray.o + t * ray.d - center;

                double phi = atan2(hit.x, hit.z);
                if (phi < 0.0)
                    phi += TWO_PI;

                if (hit.y <= radius * cos_theta_min && 	
                    hit.y >= radius * cos_theta_max && 	
                    phi >= phi_min && phi <= phi_max) {

                        tmin = t;				
                        sr.normal = (temp + t * ray.d) / radius;   // points outwards
                        sr.localHitPoint = ray.o + tmin * ray.d;				
                        return (true);
                }
            } 
        }

        return (false);
}
