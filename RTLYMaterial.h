#pragma once

#include "RTLYColor.h"
#include "RTLYUtility.h"
#include "RTLYBRDF.h"

namespace RTLY{
    class Material{
    public:
        Material(void){}						
        Material(const Material& material){}
        ~Material(){}
        virtual Material* clone(void) const = 0;	
        Material& operator= (const Material& rhs);						
        virtual RGBColor getLe(ShadeRec& sr) const{ return black;}
        virtual RGBColor shade(ShadeRec& sr){return black;}
        virtual RGBColor areaLightShade(ShadeRec& sr);
        //virtual RGBColor pathShade(ShadeRec& sr);
    };

    class Matte: public Material {	
    private:
        Lambertian*	ambient_brdf;
        Lambertian*	diffuse_brdf;
    public:
        Matte(void);											
        Matte(const Matte& m);
        virtual Material* clone(void) const;									
        Matte& operator= (const Matte& rhs);							
        ~Matte(void);											
        void setKa(const float k){ambient_brdf->setKd(k);}
        void setKd(const float k){diffuse_brdf->setKd(k);}
        void setCd(const RGBColor c){ambient_brdf->setCd(c);diffuse_brdf->setCd(c);}
        void setCd(const float r, const float g, const float b);
        void setCd(const float c);
        virtual RGBColor shade(ShadeRec& sr);
        virtual RGBColor areaLightShade(ShadeRec& sr);
    };

    class Phong : public Material{
    protected:
        Lambertian*  ambient;
        Lambertian*  diffuse;
        GlossySpecular* specular;
    public:
        Phong(void);
        Phong(const Phong& m);
        virtual Material* clone(void) const; 	
        void setKa(const float k){ambient->setKd(k);}
        void setKd(const float k){diffuse->setKd(k);}
        void setKs(const float k){specular->setKs(k);}
        void setExp(const float exp){specular->setExp(exp);}
        void setCd(const RGBColor c){ambient->setCd(c);diffuse->setCd(c);specular->setCs(c);}
        virtual RGBColor shade(ShadeRec& s);
    };
    class Emissive: public Material {	
    private:
        float		ls;		// radiance scaling factor						 
        RGBColor 	ce;		// color
    public:
        Emissive(void);
        Emissive(const  Emissive& m);
        Emissive::~Emissive(void){}
        virtual Material* clone(void) const; 	
        Emissive& operator= (const Emissive& rhs);							
        void scaleRadiance(const float _ls){ls = _ls;}
        void setCe(const float r, const float g, const float b){
            ce.r = r; ce.g = g; ce.b = b;
        }
        virtual RGBColor shade(ShadeRec& sr);
        virtual RGBColor getLe(ShadeRec& sr) const{return ls*ce;}
        virtual RGBColor areaLightShade(ShadeRec& sr);	
    };

}