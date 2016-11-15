#pragma once

#include "RTLYConstant.h"
#include "RTLYUtility.h"
#include "RTLYWorld.h"

namespace RTLY{
    class Tracer{
    public:
        Tracer():world(NULL){}
        Tracer(World* w):world(w){}
        virtual ~Tracer(){
            if(world)
                world = NULL;
        }
        virtual RGBColor traceRay(const Ray& ray) const{
            return black;
        }
        virtual RGBColor traceRay(const Ray ray,const int depth) const{
            return black;
        }
        virtual RGBColor traceRay(const Ray ray, float& tmin, const int depth) const{
            return black;
        }
    protected:
        World* world;
    };
    class SingleSphere: public Tracer {
    public:
        SingleSphere():Tracer(){}
        SingleSphere(World* w):Tracer(w){}
        virtual ~SingleSphere(){}

        virtual RGBColor traceRay(const Ray& ray) const;

    };
    class MultipleObjects : public Tracer {
    public:
        MultipleObjects():Tracer(){}
        MultipleObjects(World* w):Tracer(w){}
        virtual ~MultipleObjects(){}

        virtual RGBColor traceRay(const Ray& ray) const;

    };
    class RayCast : public Tracer {
    public:
        RayCast():Tracer(){}
        RayCast(World* w):Tracer(w){}
        virtual ~RayCast(){}
        virtual RGBColor traceRay(const Ray ray, const int depth) const;
    };
    class AreaLighting: public Tracer {
    public:
        AreaLighting():Tracer(){}
        AreaLighting(World* w):Tracer(w){}
        virtual ~AreaLighting(void){}	
        virtual RGBColor traceRay(const Ray ray, const int depth) const;
    };

}