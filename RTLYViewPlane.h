#pragma once
#include"RTLYSampler.h"

namespace RTLY{

    class ViewPlane {
    public:
        int 			hres;
        int 			vres;
        float			pixelSize;
        int				sampleCount;
        float			gamma;
        float			invGamma;
        bool			gamutShow;
        int             maxDepth;
        Sampler*        sampler;
    public:
        ViewPlane():hres(400), vres(400), pixelSize(1.0), sampleCount(1), gamma(1.0), invGamma(1.0),
            gamutShow(false), sampler(NULL){}
        ViewPlane(const ViewPlane& vp):hres(vp.hres), vres(vp.vres), pixelSize(vp.pixelSize), 
            sampleCount(vp.sampleCount), gamma(vp.gamma), invGamma(vp.invGamma), gamutShow(vp.gamutShow), 
            sampler(vp.sampler){}
        ViewPlane& operator= (const ViewPlane& rhs);
        ~ViewPlane(){}

        void setHres(const int hres){ this->hres = hres; }
        void setVres(const int vres){ this->vres = vres; }
        void setPixelSize(const float size){ pixelSize = size; }
        void setGamma(const float g){ gamma = g; }
        void setGamutShow(const bool show){  gamutShow = show;}
        void setSampleCount(const int n);			
        void setSampler(Sampler* sp);
    };

}


