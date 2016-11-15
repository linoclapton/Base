#pragma once

#include "RTLYUtility.h"
#include "RTLYSampler.h"

namespace RTLY{
    class BRDF{
    public:
        BRDF(void);						
        BRDF(const BRDF& brdf);		
        BRDF& operator=(const BRDF& rhs);
        virtual ~BRDF(void);
        void setSampler(Sampler* s);
        virtual RGBColor f(const ShadeRec& sr, const Vector3D& wi, const Vector3D& wo) const = 0;
        //virtual RGBColor sample_f(const ShadeRec& sr, const Vector3D& wi, Vector3D& wo) const = 0;
        virtual RGBColor sample_f(const ShadeRec& sr, const Vector3D& wi, Vector3D& wo, float& pdf) const = 0;
        virtual RGBColor rho(const ShadeRec& sr, const Vector3D& wo) const = 0;
    protected:
        Sampler* sampler;		// for indirect illumination
    };

    class Lambertian : public BRDF{
    public:
        Lambertian();
        ~Lambertian();
        Lambertian(const Lambertian& brdf);
        Lambertian& operator=(const Lambertian& rhs);
        Lambertian* clone() const;
        virtual RGBColor f(const ShadeRec& sr, const Vector3D& wi, const Vector3D& wo) const;
        virtual RGBColor sample_f(const ShadeRec& sr, const Vector3D& wi, Vector3D& wo, float& pdf) const;
        //virtual RGBColor sample_f(const ShadeRec& sr, const Vector3D& wi, Vector3D& wo) const;
        virtual RGBColor rho(const ShadeRec& sr, const Vector3D& wo) const;
        void setKd(const float kd){this->kd = kd;}
        void setCd(const RGBColor cd){this->cd = cd;}
    private:
        float kd;
        RGBColor cd;
    };

    class GlossySpecular: public BRDF {
    public:
        GlossySpecular(void);
        ~GlossySpecular(void);
        virtual GlossySpecular* clone(void) const;
        virtual RGBColor f(const ShadeRec& sr, const Vector3D& wo, const Vector3D& wi) const;
        virtual RGBColor sample_f(const ShadeRec& sr, const Vector3D& wo, Vector3D& wi, float& pdf) const;
        virtual RGBColor rho(const ShadeRec& sr, const Vector3D& wo) const;
        void setKs(const float ks){this->ks = ks;}
        void setExp(const float exp){this->exp = exp;}
        void setCs(const RGBColor& c){this->cs = c;}
        void setCs(const float r, const float g, const float b){
            this->cs.r = r; this->cs.g = g; this->cs.b = b;
        }
        void setCs(const float c){
            this->cs.r = c; this->cs.g = c; this->cs.b = c;
        }
        void setSampler(Sampler* sp, const float exp);   			// any type of sampling
        void setSamples(const int num_samples, const float exp); 	// multi jittered sampling
        void setNormal(const Normal& n);
    private:
        float		ks;
        RGBColor 	cs;			// specular color
        float		exp; 		// specular exponent
        Sampler*	sampler;    // for use in sample_f
    };

}
