#pragma once

#include "RTLYWorld.h"
#include "RTLYTracer.h"

namespace RTLY{
    class Camera{
    public:
        Camera();
        Camera(const Camera& c);
        Camera& operator=(const Camera& rhs);
        ~Camera();
        void computeUVW();
        virtual void render(World& w) = 0;
        void setEye(const Vector3D& eye);
        void setEye(const double x,const double y,const double z);
        void setLookAt(const Vector3D& lookAt);
        void setLookAt(const double x,const double y,const double z);
        void setUpVector(const float x, const float y, const float z);
        void setRoll(const float ra);
        void setExposureTime(const float exposure);
        //void renderStereo(World& w, float x, int pixelOffset);
    protected:
        Point3D eye;
        Point3D lookAt;
        Vector3D up;
        Vector3D u, v, w;
        float exposureTime;
        float ra;//roll angle
    };

    class PinHole : public Camera{
    public:
        PinHole();
        PinHole(const PinHole& rhs);
        Vector3D rayDirection(const Point2D& p) const;
        virtual void render(World& w);
        void setDistance(float d){this->d = d;}
    private:
        float d;
        float zoom;
    };

    class ThinLens : public Camera{
    public:
        ThinLens();
        ThinLens(const ThinLens& ph);
        void setSampler(Sampler* sp);
        Vector3D rayDirection(const Point2D& pixel,const Point2D& lens) const;
        virtual void render(World& w);
        void setViewDistance(const float vpd);
        void setZoom(float zoom_factor);
        void setLensRadius(float len_radius);
        void setFocalDistance(float dist);
    private:
        float lensRadius;
        float d;//view plane distance
        float f;//focal
        float zoom;
        Sampler* sampler;
    };

    class FishEye : public Camera{
    public:
        FishEye();
        FishEye(const FishEye& fe);
        Vector3D rayDirection(const Point2D& pp, const int hres,
            const int vres, const float	s, float& r2) const;
        virtual void render(World& w);
        void setFov(float fov){ psiMax = fov; }
    private:
        float psiMax;
    };

    class Spherical : public Camera {
    public:
        Vector3D rayDirection(const Point2D& pp, const int 	hres, 
            const int	vres, const float s ) const;

        virtual void render(World& w);
        void setHorizontalFov(float fov){lambdaMax = fov;}
        void setVerticalFov(float fov){psiMax = fov;}

        Spherical();
        Spherical(const Spherical& fe);
    private:
        float psiMax;	// in degrees
        float lambdaMax; // in degrees
    };

    typedef enum {
        parallel,
        transverse
    } ViewingType;

    class StereoCamera : public Camera {
    public:
        StereoCamera();
        StereoCamera(const StereoCamera& fe);

        virtual void render(World& w);

        void setLeftCamera(Camera* cam_ptr);
        void setRightCamera(Camera* camera_ptr);
        void useParallelViewing();
        void useTransverseViewing();
        void setPixelGap(int gap);
        void setStereoAngle(float angle);
        void setupCameras(void);
    private:

        ViewingType	type;		// parallel or transverse viewing
        int			pixelGap;			// gap in pixels between the left and right images
        float		beta;				// stereo separation angle	
        Camera*		leftCamera;	// left eye camera
        Camera*		rightCamera;	// right eye camera
    };
}