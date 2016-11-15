#include "RTLYTracer.h"
#include "RTLYMaterial.h"
#include "RTLYLight.h"

using namespace RTLY;

RGBColor SingleSphere::traceRay(const Ray& ray) const{
    ShadeRec	sr(*world); 	// not used
    double    	t;  				// not used
    if((world->sphere).hit(ray,t,sr))
        return red;
    else
        return black;
}

//----------------------------------------------------------------------------------------

RGBColor MultipleObjects::traceRay(const Ray& ray) const{
    ShadeRec	sr(world->hitObjects(ray)); 	// not used
    if(sr.hitAnObject)
        return sr.color;
    else
        return world->background;
}

//----------------------------------------------------------------------------------------

RGBColor RayCast::traceRay(const Ray ray,const int depth) const{
    ShadeRec sr(world->hitObjects(ray));

    if(sr.hitAnObject){
        sr.ray = ray;
        return sr.material->shade(sr);
    }else{
        return world->background;
    }
}

//----------------------------------------------------------------------------------------

RGBColor AreaLighting::traceRay(const Ray ray, const int depth) const {
        if (depth > world->vp.maxDepth)
            return (black);
        else {
            ShadeRec sr(world->hitObjects(ray));   

            if (sr.hitAnObject) {
                sr.depth = depth;
                sr.ray = ray;	
                return (sr.material->areaLightShade(sr));   
            }
            else
                return (world->background);
        }																																			
}