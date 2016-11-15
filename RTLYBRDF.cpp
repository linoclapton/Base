#include "RTLYBRDF.h"

using namespace RTLY;

BRDF::BRDF(void) 
    : sampler(NULL)
{}

BRDF::BRDF (const BRDF& brdf) {
    if(brdf.sampler)
        sampler	= brdf.sampler->clone(); 
    else  sampler = NULL;
}	

BRDF& BRDF::operator= (const BRDF& rhs) {
        if (this == &rhs)
            return (*this);

        if (sampler) {
            delete sampler;
            sampler = NULL;
        }

        if (rhs.sampler)
            sampler	= rhs.sampler->clone();

        return (*this);
}

BRDF::~BRDF(void) {
    if (sampler) {
        delete sampler;
        sampler = NULL;
    }
}  

void BRDF::setSampler(Sampler* sPtr) {
        sampler = sPtr;
        sampler->mapToHemisphere(1);  // for perfect diffuse
}

RGBColor BRDF::f(const ShadeRec& sr, const Vector3D& wo, const Vector3D& wi) const {
        return (black);
}

//RGBColor BRDF::sample_f(const ShadeRec& sr, const Vector3D& wo, Vector3D& wi) const {
//        return (black);
//}

RGBColor BRDF::sample_f(const ShadeRec& sr, const Vector3D& wo, Vector3D& wi, float& pdf) const {
        return (black);
}

RGBColor BRDF::rho(const ShadeRec& sr, const Vector3D& wo) const {
        return (black);
}

// ------------------------------------------------------------------------ sample_f

Lambertian::Lambertian(void) 
    :   BRDF(),
    kd(0.0), 
    cd(0.0)
{}

Lambertian::Lambertian(const Lambertian& lamb) 
    :   BRDF(lamb),
    kd(lamb.kd), 
    cd(lamb.cd)
{}

Lambertian& 
    Lambertian::operator= (const Lambertian& rhs) {
        if (this == &rhs)
            return (*this);

        BRDF::operator= (rhs);

        kd = rhs.kd; 
        cd = rhs.cd;

        return (*this);
}

Lambertian::~Lambertian(void) {}


Lambertian* 
    Lambertian::clone(void) const {
        return (new Lambertian(*this));
}	



RGBColor Lambertian::f(const ShadeRec& sr, const Vector3D& wi, const Vector3D& wo) const {
    return kd * cd * invPI;
}

RGBColor Lambertian::sample_f(const ShadeRec& sr, const Vector3D& wo, Vector3D& wi, float& pdf) const {

        Vector3D w = sr.normal;
        Vector3D v = Vector3D(0.0034, 1, 0.0071) ^ w;
        v.normalize();
        Vector3D u = v ^ w;
        Point3D sp = sampler->sampleHemisphere();  
        wi = sp.x * u + sp.y * v + sp.z * w;
        wi.normalize(); 	

        pdf = sr.normal * wi * invPI;

        return (kd * cd * invPI); 
}


RGBColor Lambertian::rho(const ShadeRec& sr, const Vector3D& wo) const {
    return kd * cd;
}

// ------------------------------------------------------------------------ sample_f


GlossySpecular::GlossySpecular(void) 
    : 	ks(0.0), 
    cs(1.0),
    sampler(NULL)
{}


GlossySpecular::~GlossySpecular(void) {}


GlossySpecular* GlossySpecular::clone (void) const {
        return (new GlossySpecular(*this));
}


void GlossySpecular::setSampler(Sampler* sp, const float exp) {
        sampler = sp;
        sampler->mapToHemisphere(exp);
}

void GlossySpecular::setSamples(const int num_samples, const float exp) {
        sampler = new JitteredSampler(num_samples);
        sampler->mapToHemisphere(exp);
}

RGBColor GlossySpecular::f(const ShadeRec& sr, const Vector3D& wo, const Vector3D& wi) const {
        RGBColor 	L;  				
        float 		ndotwi = sr.normal * wi;
        Vector3D 	r(-wi + 2.0 * sr.normal * ndotwi); // mirror reflection direction
        float 		rdotwo = r * wo; 		

        if (rdotwo > 0.0)
            L = ks * cs * pow(rdotwo, exp); 

        return (L);
}

RGBColor GlossySpecular::sample_f(const ShadeRec& sr, const Vector3D& wo, Vector3D& wi, float& pdf) const {

        float ndotwo = sr.normal * wo;
        Vector3D r = -wo + 2.0 * sr.normal * ndotwo;     // direction of mirror reflection

        Vector3D w = r;								
        Vector3D u = Vector3D(0.00424, 1, 0.00764) ^ w; 
        u.normalize();
        Vector3D v = u ^ w;

        Point3D sp = sampler->sampleHemisphere();
        wi = sp.x * u + sp.y * v + sp.z * w;			// reflected ray direction

        if (sr.normal * wi < 0.0) 						// reflected ray is below tangent plane
            wi = -sp.x * u - sp.y * v + sp.z * w;

        float phong_lobe = pow(r * wi, exp);
        pdf = phong_lobe * (sr.normal * wi);

        return (ks * cs * phong_lobe);
}

RGBColor GlossySpecular::rho(const ShadeRec& sr, const Vector3D& wo) const {
        return (black);
}

// ----------------------------------------------------------------------------------- rho
