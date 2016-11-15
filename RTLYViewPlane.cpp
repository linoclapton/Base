#include "RTLYViewPlane.h"
using namespace RTLY;

ViewPlane& ViewPlane::operator= (const ViewPlane& rhs) {
        if (this == &rhs)
            return (*this);
        hres 				= rhs.hres;
        vres 				= rhs.vres;
        pixelSize			= rhs.pixelSize;
        sampleCount 		= rhs.sampleCount;
        gamma				= rhs.gamma;
        invGamma			= rhs.invGamma;
        gamutShow	        = rhs.gamutShow;
        sampler             = rhs.sampler;
        return (*this);
}
void ViewPlane::setSampler(Sampler* sp){
    if(sampler){
        delete sampler;
        sampler = NULL;
    }
    sampleCount = sp->getSampleCount();
    sampler = sp;
}
void ViewPlane::setSampleCount(const int n){
    sampleCount = n;
    if(sampler){
        delete sampler;
        sampler = NULL;
    }

    if(sampleCount > 1){
        sampler = new JitteredSampler(n);
    }else{
        //sampler = new RegularSampler(1);
    }

}	
