#include "RTLYLight.h"
#include "RTLYWorld.h"
#include "RTLYMaterial.h"

using namespace RTLY;

Light::Light(void) {}

Light::Light(const Light& ls) {}

Light& Light::operator= (const Light& rhs) {
        if (this == &rhs)
            return (*this);
        return (*this);
}

Light::~Light(void) {} 

RGBColor Light::L(ShadeRec& s) {
    return (black);
}

bool Light::inShadow(const Ray& ray,const ShadeRec& sr){
    return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

PointLight::PointLight(const PointLight& l){
    if( this == &l )
        return;
    this->ls = l.ls;
    this->color = l.color;
    this->location = l.location;
}

PointLight* PointLight::clone() const{
    return new PointLight(*this);
}

Vector3D PointLight::getDirection(ShadeRec& sr){
     return (location-sr.localHitPoint).hat();
}

RGBColor PointLight::L(ShadeRec& sr){
    return ls * color;
}

bool PointLight::inShadow(const Ray& ray,const ShadeRec& sr){
    if(!shadow)
        return false;
    else{
        float t;
        int numObjects = sr.w.objects.size();
        float d = location.distance(ray.o);

        for (int j = 0; j < numObjects; j++)
            if (sr.w.objects[j]->shadowHit(ray, t) && t < d)
                return (true); 

        return (false);     
    }
}

//------------------------------------------------------------------------------- scale_radiance

Ambient::Ambient (void)
    : 	Light(),
    ls(1.0),
    color(1.0)			
{}

Ambient::Ambient (const Ambient& a)
    : 	Light(a),
    ls(a.ls),
    color(a.color) 		
{}

Light* Ambient::clone(void) const {
     return (new Ambient(*this));
}	

Ambient& Ambient::operator= (const Ambient& rhs) {
    if (this == &rhs)
        return (*this);
    Light::operator= (rhs);
    ls 		= rhs.ls;
    color 	= rhs.color;
    return (*this);
}

Ambient::~Ambient (void) {}

Vector3D Ambient::getDirection(ShadeRec& s) {
    return (Vector3D(0.0));
}

RGBColor Ambient::L(ShadeRec& sr) {	
    return (ls * color);
}

//------------------------------------------------------------------------------- scale_radiance


Directional::Directional(void)
    : 	Light(),
    ls(1.0),
    color(1.0),
    dir(0, 1, 0)			
{}

Directional::Directional(const Directional& dl)
    : 	Light(dl),
    ls(dl.ls),
    color(dl.color),
    dir(dl.dir)  		
{}

Light* Directional::clone(void) const {
        return (new Directional(*this));
}

Directional& Directional::operator= (const Directional& rhs) 	
{
    if (this == &rhs)
        return (*this);

    Light::operator= (rhs);

    ls		= rhs.ls;
    color 	= rhs.color;
    dir 	= rhs.dir;

    return (*this);
}

Directional::~Directional(void) {}

Vector3D Directional::getDirection(ShadeRec& sr) {
        return (dir);
}	

RGBColor Directional::L(ShadeRec& s) {	
        return (ls * color);
}

//------------------------------------------------------------------------------------------------------------------------------
AmbientOccluder::AmbientOccluder(void)
    : 	Light(),
    ls(1.0),
    color(1.0),
    sampler(NULL)
{}

AmbientOccluder::AmbientOccluder(const AmbientOccluder& dl)
    : 	Light(dl),
    ls(dl.ls),
    color(dl.color),
    sampler(NULL)
{}

Light* AmbientOccluder::clone(void) const {
    return (new AmbientOccluder(*this));
}

AmbientOccluder& AmbientOccluder::operator= (const AmbientOccluder& rhs) 	
{
    if (this == &rhs)
        return (*this);

    Light::operator= (rhs);

    ls		= rhs.ls;
    color 	= rhs.color;
    u       = rhs.u;
    v       = rhs.v;
    w       = rhs.w;
    sampler = rhs.sampler;
    minAmount = minAmount;

    return (*this);
}
AmbientOccluder::~AmbientOccluder(){};


void AmbientOccluder::setSampler(Sampler* s_ptr) {
        if (sampler) {
            delete sampler;
            sampler = NULL;
        }
        sampler = s_ptr;
        sampler->mapToHemisphere(1);
}

Vector3D AmbientOccluder::getDirection(ShadeRec& sr) {
        Point3D sp = sampler->sampleHemisphere();
        return (sp.x * u + sp.y * v + sp.z * w);
}	

bool AmbientOccluder::inShadow(const Ray& ray, const ShadeRec& sr) {			
        float 	t;
        int 	num_objects = sr.w.objects.size();

        for (int j = 0; j < num_objects; j++)
            if (sr.w.objects[j]->shadowHit(ray, t))
                return (true);  

        return (false);  
}

RGBColor AmbientOccluder::L(ShadeRec& sr) {	
        w = sr.normal;	
        v = w ^ Vector3D(0.0072, 1.0, 0.0034); // jitter the up vector in case normal is vertical
        v.normalize();
        u = v ^ w;

        Ray shadow_ray;
        shadow_ray.o = sr.hitPoint;
        shadow_ray.d = getDirection(sr);  

        if (inShadow(shadow_ray, sr))
            return (minAmount * ls * color);
        else	
            return (ls * color);
}

//----------------------------------------------------------------------------------------

AreaLight::AreaLight(void)
    : 	Light(),
    object_ptr(NULL),
    material_ptr(NULL)
{}	


AreaLight::AreaLight(const AreaLight& al)
    : 	Light(al) {
        if(al.object_ptr)
            //TODO:object_ptr = al.object_ptr->clone(); 
            object_ptr = al.object_ptr;
        else  
            object_ptr = NULL;

        if(al.material_ptr)
            material_ptr = al.material_ptr->clone(); 
        else  material_ptr = NULL;
}



Light* AreaLight::clone() const{
        return (new AreaLight(*this));
}					



AreaLight::~AreaLight(void) {
    if (object_ptr) {
        delete object_ptr;
        object_ptr = NULL;
    }

    if (material_ptr) {
        delete material_ptr;
        material_ptr = NULL;
    }
}

AreaLight& AreaLight::operator= (const AreaLight& rhs) {
        if (this == &rhs)
            return (*this);

        Light::operator=(rhs);

        if (object_ptr) {
            delete object_ptr;
            object_ptr = NULL;
        }
        if (rhs.object_ptr)
        //TODO: object_ptr = rhs.object_ptr->clone();
            object_ptr = rhs.object_ptr;

        if (material_ptr) {
            delete material_ptr;
            material_ptr = NULL;
        }

        if (rhs.material_ptr)
            material_ptr = rhs.material_ptr->clone();

        return (*this);
}


Vector3D AreaLight::getDirection(ShadeRec& sr) {
    sample_point = object_ptr->sample();    // used in the G function
    light_normal = object_ptr->getNormal(sample_point); 
    wi = sample_point - sr.hitPoint;  		// used in the G function
    wi.normalize();
    return (wi);
}

RGBColor AreaLight::L(ShadeRec& sr) {
    float ndotd = -light_normal * wi; 

    if (ndotd > 0.0)		
        return (material_ptr->getLe(sr)); 
    else
        return (black);
}

void AreaLight::setObject(GeoObject* obj_ptr){
    object_ptr = obj_ptr;
}

bool AreaLight::inShadow(const Ray& ray, const ShadeRec& sr) {
        float t;
        int num_objects = sr.w.objects.size();
        float ts = (sample_point - ray.o) * ray.d;

        for (int j = 0; j < num_objects; j++)
            if (sr.w.objects[j]->shadowHit(ray, t) && t < ts)
                return (true); 

        return (false);     
}

float AreaLight::G(const ShadeRec& sr) const {
        float ndotd = -light_normal * wi;
        float d2 	= sample_point.distance2(sr.hitPoint);

        return (ndotd / d2);
}

float AreaLight::pdf(const ShadeRec& sr) const {
        return (object_ptr->pdf(sr));
}

//----------------------------------------------------------------------------------------

EnvironmentLight::EnvironmentLight(void)
    : Light(), sampler_ptr(NULL), material_ptr(NULL)
{}

EnvironmentLight::EnvironmentLight(const EnvironmentLight& al)
    : Light(al) {
        if (al.sampler_ptr)
            sampler_ptr = al.sampler_ptr->clone();
        else  sampler_ptr = NULL;

        if (al.material_ptr)
            material_ptr = al.material_ptr->clone();
        else  material_ptr = NULL;
}

Light* EnvironmentLight::clone(void) const{
    return (new EnvironmentLight(*this));
}

EnvironmentLight::~EnvironmentLight(void) {
    if (sampler_ptr) {
        delete sampler_ptr;
        sampler_ptr = NULL;
    }

    if (material_ptr) {
        delete material_ptr;
        material_ptr = NULL;
    }
}


EnvironmentLight& EnvironmentLight::operator= (const EnvironmentLight& rhs){
    if (this == &rhs)
        return (*this);

    Light::operator=(rhs);

    if (sampler_ptr) {
        delete sampler_ptr;
        sampler_ptr = NULL;
    }

    if (rhs.sampler_ptr)
        sampler_ptr = rhs.sampler_ptr->clone();

    if (material_ptr) {
        delete material_ptr;
        material_ptr = NULL;
    }

    if (rhs.material_ptr)
        material_ptr = rhs.material_ptr->clone();

    return (*this);
}

void EnvironmentLight::setSampler(Sampler* s_ptr){
    if (sampler_ptr) {
        delete sampler_ptr;
        sampler_ptr = NULL;
    }

    sampler_ptr = s_ptr;
    sampler_ptr->mapToHemisphere(1);

}

void EnvironmentLight::setMaterial(Material* mat_ptr){
    if (material_ptr != NULL){
        delete material_ptr;
        material_ptr = NULL;
    }
    material_ptr = mat_ptr;
}


Vector3D EnvironmentLight::getDirection(ShadeRec& sr) {
    w = sr.normal;
    v = w ^ Vector3D(0.0072, 1.0, 0.0034); // jitter the up vector in case normal is vertical
    v.normalize();
    u = v ^ w;

    Point3D sp = sampler_ptr->sampleHemisphere();
    wi = sp.x * u + sp.y * v + sp.z * w;

    return (wi);
}

RGBColor EnvironmentLight::L(ShadeRec& sr) {
    return (material_ptr->getLe(sr));
}

float EnvironmentLight::pdf(const ShadeRec& sr) const{
    return (sr.normal * wi * invPI);
}

bool EnvironmentLight::inShadow(const Ray& ray, const ShadeRec& sr){
    float 	t;
    int 	num_objects = sr.w.objects.size();
    for (int j = 0; j < num_objects; j++)
    {
        if (sr.w.objects[j]->shadowHit(ray, t))
        {
            return (true);
        }
    }
    return (false);
}