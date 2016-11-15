#include "RTLYMaterial.h"
#include "RTLYWorld.h"
#include "RTLYLight.h"

using namespace RTLY;


Material& Material::operator= (const Material& rhs) {
        if (this == &rhs)
            return (*this);
        return (*this);
}

RGBColor Material::areaLightShade(ShadeRec& sr){
    return black;
}

//----------------------------------------------------------------------------------------------------

Matte::Matte (void)
    :	Material(),
    ambient_brdf(new Lambertian),
    diffuse_brdf(new Lambertian)
{}

Matte::Matte(const Matte& m)
    : 	Material(m)
{
    if(m.ambient_brdf)
        ambient_brdf = m.ambient_brdf->clone(); 
    else  ambient_brdf = NULL;

    if(m.diffuse_brdf)
        diffuse_brdf = m.diffuse_brdf->clone(); 
    else  diffuse_brdf = NULL;
}

Material* Matte::clone(void) const {
        return (new Matte(*this));
}	

Matte& Matte::operator= (const Matte& rhs) {
        if (this == &rhs)
            return (*this);

        Material::operator=(rhs);

        if (ambient_brdf) {
            delete ambient_brdf;
            ambient_brdf = NULL;
        }

        if (rhs.ambient_brdf)
            ambient_brdf = rhs.ambient_brdf->clone();

        if (diffuse_brdf) {
            delete diffuse_brdf;
            diffuse_brdf = NULL;
        }

        if (rhs.diffuse_brdf)
            diffuse_brdf = rhs.diffuse_brdf->clone();

        return (*this);
}

Matte::~Matte(void) {

    if (ambient_brdf) {
        delete ambient_brdf;
        ambient_brdf = NULL;
    }

    if (diffuse_brdf) {
        delete diffuse_brdf;
        diffuse_brdf = NULL;
    }
}

void Matte::setCd(const float r,const float g,const float b){
    ambient_brdf->setCd(RGBColor(r,g,b));
    diffuse_brdf->setCd(RGBColor(r,g,b));
}

void Matte::setCd(const float c){
    ambient_brdf->setCd(RGBColor(c));
    diffuse_brdf->setCd(RGBColor(c));
}

RGBColor Matte::shade(ShadeRec& sr) {
    Vector3D 	wo 			= -sr.ray.d;
    RGBColor 	L 			= ambient_brdf->rho(sr, wo) * sr.w.ambient->L(sr);
    int 		num_lights	= sr.w.lights.size();
    for (int j = 0; j < num_lights; j++) {
        Vector3D wi = sr.w.lights[j]->getDirection(sr);    
        float ndotwi = sr.normal * wi;
        if (ndotwi > 0.0) 
            L += diffuse_brdf->f(sr, wo, wi) * sr.w.lights[j]->L(sr) * ndotwi;
    }
    return (L);
}

RGBColor Matte::areaLightShade(ShadeRec& sr) {
    Vector3D 	wo = -sr.ray.d;
    RGBColor 	L = ambient_brdf->rho(sr, wo) * sr.w.ambient->L(sr);
    int 		num_lights = sr.w.lights.size();

    for (int j = 0; j < num_lights; j++) {
        Vector3D 	wi = sr.w.lights[j]->getDirection(sr);
        float 		ndotwi = sr.normal * wi;

        if (ndotwi > 0.0) {
            bool in_shadow = false;

            if (sr.w.lights[j]->castShadow()) {
                Ray shadow_ray(sr.hitPoint, wi);
                in_shadow = sr.w.lights[j]->inShadow(shadow_ray, sr);
            }

            if (!in_shadow)
                L += diffuse_brdf->f(sr, wo, wi) * sr.w.lights[j]->L(sr) * sr.w.lights[j]->G(sr) * ndotwi / sr.w.lights[j]->pdf(sr);
        }
    }

    return (L);
}

//-----------------------------------------------------------------------------------------

Phong::Phong():Material(),ambient(new Lambertian),diffuse(new Lambertian),specular(new GlossySpecular){

}

RGBColor Phong::shade(ShadeRec& s){
    Vector3D 	wo 			= -s.ray.d;
    RGBColor 	L 			= ambient->rho(s, wo) * s.w.ambient->L(s);
    int 		num_lights	= s.w.lights.size();
    for (int j = 0; j < num_lights; j++) {
        Vector3D wi = s.w.lights[j]->getDirection(s);    
        float ndotwi = s.normal * wi;
        if (ndotwi > 0.0){ 
            bool inShadow = false;
            if (s.w.lights[j]->castShadow()) {
                Ray shadowRay(s.hitPoint, wi);
                inShadow = s.w.lights[j]->inShadow(shadowRay, s);
            }

            if(!inShadow)
            L += (diffuse->f(s, wo, wi) + specular->f(s,wo,wi))
                * s.w.lights[j]->L(s) * ndotwi;
        }
    }
    return (L);
}
Phong::Phong(const Phong& m):Material(m){
    if( this == &m)
        return;
    if(m.ambient)
        ambient = m.ambient->clone(); 
    else  
        ambient = NULL;

    if(m.diffuse)
        diffuse = m.diffuse->clone(); 
    else  
        diffuse = NULL;

    if(m.specular)
        specular =m.specular->clone();
    else
        specular = NULL;
}

Material* Phong::clone() const{
    return new Phong(*this);
}

//-----------------------------------------------------------------------------------------

RGBColor Emissive::areaLightShade(ShadeRec& sr){
    if (-sr.normal * sr.ray.d > 0.0)
        return (ls * ce);
    else
        return (black);
}

Emissive::Emissive(void) : Material(), ls(1), ce(1.0)
{}

Emissive::Emissive(const Emissive& m) : Material(m), ls(m.ls), ce(m.ce)
{}

Material* Emissive::clone(void) const {
    return (new Emissive(*this));
}

Emissive& Emissive::operator= (const Emissive& rhs){
    if (this == &rhs)
        return (*this);

    Material::operator=(rhs);
    ls = rhs.ls;
    ce = rhs.ce;
    return (*this);
}



RGBColor Emissive::shade(ShadeRec& sr){
    // when the incoming ray is on the same side of the object surface as the normal
    if (-sr.normal * sr.ray.d > 0.0)
        return (ls * ce);
    else
        return (black);
}