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

//----------------------------------------------------------------------------------------------------

Matrix Instance::forward_matrix;

Instance::Instance(void) : GeoObject(),
    object_ptr(NULL),
    inv_matrix(),
    bbox(),
    transform_the_texture(true)
{
    forward_matrix.setIdentity();
}

Instance::Instance(GeoObject* obj_ptr)     
    :	GeoObject(),
    object_ptr(obj_ptr),
    inv_matrix(),
    bbox(),
    transform_the_texture(true) 
{
    forward_matrix.setIdentity();
}	

Instance::Instance (const Instance& instance)
    : 	GeoObject(instance),
    inv_matrix(instance.inv_matrix),
    transform_the_texture(instance.transform_the_texture)
{
    if(instance.object_ptr)
        object_ptr = instance.object_ptr->clone(); 
    else  object_ptr = NULL;
}

Instance* Instance::clone(void) const {
    return (new Instance(*this));
}

Instance::~Instance(void) {
    if (object_ptr) {
        delete object_ptr;
        object_ptr = NULL;
    }
}

Instance& Instance::operator= (const Instance& rhs) {
        if (this == &rhs)
            return (*this);

        GeoObject::operator=(rhs);

        if(object_ptr) {
            delete object_ptr;
            object_ptr = NULL;
        }

        if (rhs.object_ptr)
            object_ptr = rhs.object_ptr->clone();
        else
            object_ptr = NULL;

        inv_matrix				= rhs.inv_matrix;
        bbox					= rhs.bbox;
        transform_the_texture 	= rhs.transform_the_texture;

        return (*this);
}

void Instance::computeBoundingBox(void) {

        // First get the object's untransformed BBox

        BBox object_bbox = object_ptr->getBoundingBox();


        // Now apply the affine transformations to the box.
        // We must apply the transformations to all 8 vertices of the orginal box
        // and then work out the new minimum and maximum values

        // Construct the eight vertices as 3D points:

        Point3D v[8];

        v[0].x = object_bbox.x0; v[0].y = object_bbox.y0; v[0].z = object_bbox.z0;
        v[1].x = object_bbox.x1; v[1].y = object_bbox.y0; v[1].z = object_bbox.z0;
        v[2].x = object_bbox.x1; v[2].y = object_bbox.y1; v[2].z = object_bbox.z0;
        v[3].x = object_bbox.x0; v[3].y = object_bbox.y1; v[3].z = object_bbox.z0;

        v[4].x = object_bbox.x0; v[4].y = object_bbox.y0; v[4].z = object_bbox.z1;
        v[5].x = object_bbox.x1; v[5].y = object_bbox.y0; v[5].z = object_bbox.z1;
        v[6].x = object_bbox.x1; v[6].y = object_bbox.y1; v[6].z = object_bbox.z1;
        v[7].x = object_bbox.x0; v[7].y = object_bbox.y1; v[7].z = object_bbox.z1;


        // Transform these using the forward matrix

        v[0] = forward_matrix * v[0];
        v[1] = forward_matrix * v[1];
        v[2] = forward_matrix * v[2];
        v[3] = forward_matrix * v[3];
        v[4] = forward_matrix * v[4];
        v[5] = forward_matrix * v[5];
        v[6] = forward_matrix * v[6];
        v[7] = forward_matrix * v[7];


        // Since forward_matrix is a static variable, we must now set it to the unit matrix
        // This sets it up correctly for the next instance object

        forward_matrix.setIdentity();


        // Compute the minimum values

        float x0 = kHugeValue;
        float y0 = kHugeValue;
        float z0 = kHugeValue;

        for (int j = 0; j <= 7; j++)  {
            if (v[j].x < x0)
                x0 = v[j].x;
        }

        for (int j = 0; j <= 7; j++) {
            if (v[j].y < y0)
                y0 = v[j].y;
        }

        for (int j = 0; j <= 7; j++) {
            if (v[j].z < z0)
                z0 = v[j].z;
        }

        // Compute the minimum values

        float x1 = -kHugeValue;
        float y1 = -kHugeValue;
        float z1 = -kHugeValue;   

        for (int j = 0; j <= 7; j++) {
            if (v[j].x > x1)
                x1 = v[j].x;
        }

        for (int j = 0; j <= 7; j++) {
            if (v[j].y > y1)
                y1 = v[j].y;
        }

        for (int j = 0; j <= 7; j++) {
            if (v[j].z > z1)
                z1 = v[j].z;
        }

        // Assign values to the bounding box

        bbox.x0 = x0;
        bbox.y0 = y0;
        bbox.z0 = z0;
        bbox.x1 = x1;
        bbox.y1 = y1;
        bbox.z1 = z1;	
}

BBox Instance::getBoundingBox(void) {
        return (bbox);
}

Material* Instance::getMaterial(void) const {
        return (material);     
}

void Instance::setMaterial(Material* m_ptr) {
        material = m_ptr;
}

bool Instance::hit(const Ray& ray, double& t, ShadeRec& sr) const {
        Ray inv_ray(ray);  
        inv_ray.o = inv_matrix * inv_ray.o;   
        inv_ray.d = inv_matrix * inv_ray.d;

        if (object_ptr->hit(inv_ray, t, sr)) {
            sr.normal = inv_matrix * sr.normal;
            sr.normal.normalize();				

            if (object_ptr->getMaterial())
                ;//sr.material = object_ptr->getMaterial(); 

            if (!transform_the_texture) 
                sr.localHitPoint = ray.o + t * ray.d;  		 

            return (true);
        } 

        return (false);   
}

void Instance::scale(const Vector3D& s) {

        Matrix	inv_scaling_matrix;			// temporary inverse scaling matrix

        inv_scaling_matrix.m[0][0] = 1.0 / s.x;
        inv_scaling_matrix.m[1][1] = 1.0 / s.y;
        inv_scaling_matrix.m[2][2] = 1.0 / s.z;

        inv_matrix = inv_matrix * inv_scaling_matrix;			

        Matrix	scaling_matrix;				// temporary scaling matrix

        scaling_matrix.m[0][0] = s.x;
        scaling_matrix.m[1][1] = s.y;
        scaling_matrix.m[2][2] = s.z;

        forward_matrix = scaling_matrix * forward_matrix; 		
}

void Instance::scale(const double a, const double b, const double c) {

        Matrix inv_scaling_matrix;					// temporary inverse scaling matrix

        inv_scaling_matrix.m[0][0] = 1.0 / a;
        inv_scaling_matrix.m[1][1] = 1.0 / b;
        inv_scaling_matrix.m[2][2] = 1.0 / c;

        inv_matrix = inv_matrix * inv_scaling_matrix;			

        Matrix scaling_matrix;						// temporary scaling matrix

        scaling_matrix.m[0][0] = a;
        scaling_matrix.m[1][1] = b;
        scaling_matrix.m[2][2] = c;

        forward_matrix = scaling_matrix * forward_matrix; 	
}

void Instance::translate(const Vector3D& trans) {

        Matrix inv_translation_matrix;				// temporary inverse translation matrix	

        inv_translation_matrix.m[0][3] = -trans.x;
        inv_translation_matrix.m[1][3] = -trans.y;
        inv_translation_matrix.m[2][3] = -trans.z;

        inv_matrix = inv_matrix * inv_translation_matrix;

        Matrix translation_matrix;					// temporary translation matrix	

        translation_matrix.m[0][3] = trans.x;
        translation_matrix.m[1][3] = trans.y;
        translation_matrix.m[2][3] = trans.z;

        forward_matrix = translation_matrix * forward_matrix; 
}

void Instance::translate(const double dx, const double dy, const double dz) {

        Matrix inv_translation_matrix;				// temporary inverse translation matrix	

        inv_translation_matrix.m[0][3] = -dx;
        inv_translation_matrix.m[1][3] = -dy;
        inv_translation_matrix.m[2][3] = -dz;

        inv_matrix = inv_matrix * inv_translation_matrix;

        Matrix translation_matrix;					// temporary translation matrix	

        translation_matrix.m[0][3] = dx;
        translation_matrix.m[1][3] = dy;
        translation_matrix.m[2][3] = dz;

        forward_matrix = translation_matrix * forward_matrix; 
}

void Instance::rotateX(const double theta) {

        double sin_theta = sin(theta * PI_ON_180);
        double cos_theta = cos(theta * PI_ON_180);

        Matrix inv_x_rotation_matrix;					// temporary inverse rotation matrix about x axis

        inv_x_rotation_matrix.m[1][1] = cos_theta;
        inv_x_rotation_matrix.m[1][2] = sin_theta;   	
        inv_x_rotation_matrix.m[2][1] = -sin_theta;  
        inv_x_rotation_matrix.m[2][2] = cos_theta;		

        inv_matrix = inv_matrix * inv_x_rotation_matrix;	   	

        Matrix x_rotation_matrix;						// temporary rotation matrix about x axis

        x_rotation_matrix.m[1][1] = cos_theta;
        x_rotation_matrix.m[1][2] = -sin_theta;
        x_rotation_matrix.m[2][1] = sin_theta;
        x_rotation_matrix.m[2][2] = cos_theta;

        forward_matrix = x_rotation_matrix * forward_matrix; 
}

void Instance::rotateY(const double theta) {

        double sin_theta = sin(theta * PI / 180.0);
        double cos_theta = cos(theta * PI / 180.0);

        Matrix inv_y_rotation_matrix;					// temporary inverse rotation matrix about y axis

        inv_y_rotation_matrix.m[0][0] = cos_theta;   
        inv_y_rotation_matrix.m[0][2] = -sin_theta;  
        inv_y_rotation_matrix.m[2][0] = sin_theta;
        inv_y_rotation_matrix.m[2][2] = cos_theta;		

        inv_matrix = inv_matrix * inv_y_rotation_matrix;	   	

        Matrix y_rotation_matrix;						// temporary rotation matrix about x axis

        y_rotation_matrix.m[0][0] = cos_theta;
        y_rotation_matrix.m[0][2] = sin_theta;
        y_rotation_matrix.m[2][0] = -sin_theta;
        y_rotation_matrix.m[2][2] = cos_theta;

        forward_matrix = y_rotation_matrix * forward_matrix; 
}

void Instance::rotateZ(const double theta) {
        double sin_theta = sin(theta * PI / 180.0);
        double cos_theta = cos(theta * PI / 180.0);

        Matrix inv_z_rotation_matrix;					// temporary inverse rotation matrix about y axis	

        inv_z_rotation_matrix.m[0][0] = cos_theta;
        inv_z_rotation_matrix.m[0][1] = sin_theta;   	
        inv_z_rotation_matrix.m[1][0] = -sin_theta;  
        inv_z_rotation_matrix.m[1][1] = cos_theta;	

        inv_matrix = inv_matrix * inv_z_rotation_matrix;

        Matrix z_rotation_matrix;						// temporary rotation matrix about y axis

        z_rotation_matrix.m[0][0] = cos_theta;
        z_rotation_matrix.m[0][1] = -sin_theta;
        z_rotation_matrix.m[1][0] = sin_theta;
        z_rotation_matrix.m[1][1] = cos_theta;

        forward_matrix = z_rotation_matrix * forward_matrix; 
}

void Instance::shear(const Matrix& s) {

        Matrix inverse_shearing_matrix;    // inverse shear matrix

        // discriminant

        double d = 1.0 	- s.m[1][0] * s.m[0][1] - s.m[2][0] * s.m[0][2]  - s.m[2][1] * s.m[1][2]
        + s.m[1][0] * s.m[2][1] * s.m[0][2] + s.m[2][0] * s.m[0][1] * s.m[2][1];

        // diagonals

        inverse_shearing_matrix.m[0][0] = 1.0 - s.m[2][1] * s.m[1][2];
        inverse_shearing_matrix.m[1][1] = 1.0 - s.m[2][0] * s.m[0][2];
        inverse_shearing_matrix.m[2][2] = 1.0 - s.m[1][0] * s.m[0][1];
        inverse_shearing_matrix.m[3][3] = d;

        // first row

        inverse_shearing_matrix.m[0][1] = -s.m[1][0] + s.m[2][0] * s.m[1][2];
        inverse_shearing_matrix.m[0][2] = -s.m[2][0] + s.m[1][0] * s.m[2][1];

        // second row

        inverse_shearing_matrix.m[1][0] = -s.m[0][1] + s.m[2][1] * s.m[0][2];
        inverse_shearing_matrix.m[1][2] = -s.m[2][1] + s.m[2][0] * s.m[0][1];

        // third row

        inverse_shearing_matrix.m[2][0] = -s.m[0][2] + s.m[0][1] * s.m[1][2];
        inverse_shearing_matrix.m[2][1] = -s.m[1][2] + s.m[1][0] * s.m[0][2] ;

        // divide by discriminant

        inverse_shearing_matrix = inverse_shearing_matrix / d;

        inv_matrix = inv_matrix * inverse_shearing_matrix;	

        forward_matrix = s * forward_matrix; 
}


