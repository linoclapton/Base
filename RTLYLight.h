#pragma once

#include "RTLYUtility.h"

namespace RTLY{
    class Sampler;
    class GeoObject;
    class Light{
    protected:
        bool shadow;
    public:
        Light(void); 
        Light(const Light& ls);			
        Light& operator= (const Light& rhs); 
        virtual Light* clone(void) const = 0;
        virtual ~Light(void);
        virtual Vector3D getDirection(ShadeRec& sr) = 0;				
        virtual RGBColor L(ShadeRec& sr);								
        void setShadow(bool shadow){this->shadow = shadow;}
        bool castShadow(){return shadow;}
        virtual bool inShadow(const Ray& ray,const ShadeRec& sr);
        virtual float G(const ShadeRec& sr) const{return 1.0;}
        virtual float pdf(const ShadeRec& sr) const{return 1.0;}
    };

    class PointLight : public Light{
    private:
        float ls;
        RGBColor color;
        Vector3D location;
    public:
        PointLight():ls(1.0),color(1.0),location(0.0){}
        PointLight(const PointLight& l);			
        virtual Vector3D getDirection(ShadeRec& sr);
        virtual RGBColor L(ShadeRec& sr);
        virtual PointLight* clone(void) const;
        void setLocation(float x,float y,float z){
            location = Vector3D(x,y,z);
        }
        void scaleRadiance(float f){ls = f;}
        virtual bool inShadow(const Ray& ray,const ShadeRec& sr);
    };

    class Ambient: public Light {
    private:
        float		ls;
        RGBColor	color;
    public:
        Ambient(void);   							
        Ambient(const Ambient& a); 					 
        virtual Light* clone(void) const;	
        Ambient& operator= (const Ambient& rhs);									
        virtual ~Ambient(void);
        void scaleRadiance(const float b){ls = b;}
        void setColor(const float c){color = RGBColor(c);}
        void setColor(const RGBColor& c){color = c;}
        void setColor(const float r, const float g, const float b)
        {color.r = r; color.g = g; color.b = b;}
        virtual Vector3D getDirection(ShadeRec& s); 
        virtual RGBColor L(ShadeRec& s);
    };

    class Directional: public Light {
    private:
        float		ls;			
        RGBColor	color;
        Vector3D	dir;		// direction the light comes from
    public:
        Directional(void);   							
        Directional(const Directional& dl); 
        virtual Light* clone(void) const;			
        Directional& operator= (const Directional& rhs); 
        virtual ~Directional(void); 
        void scaleRadiance(const float b){ls = b;}
        void setColor(const float c){color = RGBColor(c);}
        void setColor(const RGBColor& c){color = c;}
        void setColor(const float r, const float g, const float b) {
            color.r = r; color.g= g; color.b = b;
        }		
        void setDirection(Vector3D d){dir = d;dir.normalize();}					
        void setDirection(float dx, float dy, float dz){
            dir.x = dx; dir.y = dy; dir.z = dz;
            dir.normalize();
        }
        virtual Vector3D getDirection(ShadeRec& sr);
        virtual RGBColor L(ShadeRec& sr);	
    };

    class AmbientOccluder : public Light{
    private:
        float ls;
        RGBColor color;
        Vector3D u,v,w;
        Sampler* sampler;
        float    minAmount;
    public:
        AmbientOccluder();
        AmbientOccluder(const AmbientOccluder& dl); 
        virtual Light* clone() const;			
        AmbientOccluder& operator= (const AmbientOccluder& rhs); 
        virtual ~AmbientOccluder(); 

        void setColor(const RGBColor& color){this->color = color;}
        void scaleRadiance(const float b){ls = b;}
        void setSampler(Sampler* s);
        void setMinAmount(float amount){minAmount = amount;}
        virtual Vector3D getDirection(ShadeRec& sr);
        virtual bool inShadow(const Ray& ray,const ShadeRec& sr);
        virtual RGBColor L(ShadeRec& sr);	
    };

    class AreaLight: public Light {	
    private:
        GeoObject*       	object_ptr;
        Material* 			material_ptr;	 // will be an emissive material
        Point3D				sample_point;
        Normal				light_normal;    // assigned in get_direction - which therefore can't be const for any light
        Vector3D			wi;			     // unit direction from hit point being shaded to sample point on light surface			
    public:
        AreaLight(void);						
        AreaLight(const AreaLight& al);				
        virtual Light* clone(void) const;
        virtual ~AreaLight(void);
        AreaLight& operator= (const AreaLight& rhs); 
        void setObject(GeoObject* obj_ptr);
        virtual Vector3D getDirection(ShadeRec& s);
        virtual RGBColor L(ShadeRec& sr);
        virtual bool inShadow(const Ray& ray, const ShadeRec& sr);
        virtual float G(const ShadeRec& sr) const;
        virtual float pdf(const ShadeRec& sr) const;
        void setMaterial(Material* m){material_ptr = m;}
    };

    class EnvironmentLight : public Light{
    private:
        Sampler*	sampler_ptr;
        Material* 	material_ptr;
        Vector3D	u, v, w;
        Vector3D	wi;
    public:
        EnvironmentLight();
        EnvironmentLight(const EnvironmentLight& al);
        virtual Light* clone() const;
        virtual ~EnvironmentLight();
        EnvironmentLight& operator= (const EnvironmentLight& rhs);
        void setSampler(Sampler* sampler);
        virtual Vector3D getDirection(ShadeRec& s);
        virtual RGBColor L(ShadeRec& sr);
        virtual bool inShadow(const Ray& ray, const ShadeRec& sr);
        virtual float pdf(const ShadeRec& sr) const;
        void setMaterial(Material* m);
    };


}