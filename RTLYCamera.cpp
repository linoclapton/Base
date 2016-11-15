#include "RTLYCamera.h"

using namespace RTLY;

Camera::Camera(void):
    eye(0, 0, 500),
    lookAt(0),
    ra(0),
    up(0, 1, 0),
    u(1, 0, 0),
    v(0, 1, 0),
    w(0, 0, 1),
    exposureTime(1.0)
{}



Camera::Camera(const Camera& c)   		
    :eye(c.eye),
    lookAt(c.lookAt),
    ra(c.ra),
    up(c.up),
    u(c.u),
    v(c.v),
    w(c.w),
    exposureTime(c.exposureTime)
{}




Camera& Camera::operator= (const Camera& rhs) {
        if (this == &rhs)
            return (*this);
        eye				= rhs.eye;
        lookAt			= rhs.lookAt;
        ra				= rhs.ra;
        up				= rhs.up;
        u				= rhs.u;
        v				= rhs.v;
        w				= rhs.w;
        exposureTime 	= rhs.exposureTime;
        return (*this);
}



Camera::~Camera(void) {}


void Camera::computeUVW(){
    w = eye - lookAt;
    w.normalize();
    u = up ^ w;
    u.normalize();
    v = w ^ u;
}

void Camera::setEye(const double x,const double y,const double z){
    eye = Point3D(x,y,z);
}

void Camera::setEye(const Vector3D& eye){
    this->eye = eye;
}

void Camera::setLookAt(const double x,const double y,const double z){
    lookAt = Point3D(x,y,z);
}

void Camera::setLookAt(const Vector3D& lookAt){
    this->lookAt = lookAt;
}

void Camera::setUpVector(const float x,const float y,const float z){
    up = Vector3D(x,y,z);
}

void Camera::setRoll(const float ra){
    this->ra = ra;
}

void Camera::setExposureTime(const float exposure){
    this->exposureTime = exposure;
}

PinHole::PinHole(void) : Camera(), d(500), zoom(1.0)
{}

PinHole::PinHole(const PinHole& c) : Camera(c), d(c.d), zoom(c.zoom)
{}

void PinHole::render(World& w){
    RGBColor	L;
    ViewPlane	vp(w.vp);	 								
    Ray			ray;
    int 		depth = 0;  
    Point2D     sp;
    Point2D 	pp;		// sample point on a pixel

    vp.pixelSize /= zoom;
    ray.o = eye;

    for (int r = 0; r < vp.vres; r++){			// up
        for (int c = 0; c < vp.hres; c++) {		// across 					
            L = black; 

            for (int j = 0; j < vp.sampleCount; j++){
                sp = vp.sampler->sampleUnitSquare();
                pp.x = vp.pixelSize * (c - 0.5 * vp.hres + sp.x);
                pp.y = vp.pixelSize * (r - 0.5 * vp.vres + sp.y);
                ray.d = rayDirection(pp);
                L += w.tracer->traceRay(ray,depth);
            }

            L /= vp.sampleCount;
            L *= exposureTime;
            w.setPixel(r, c, L);
        } 
        system("cls");
        printf("%5.2f%%/100%%",100.0*(r+1)/vp.vres);
    }
}

ThinLens::ThinLens()
    : Camera(), d(500), f(600), lensRadius(1), zoom(1.0), sampler(NULL)
{}

ThinLens::ThinLens(const ThinLens& ph)
    : Camera(ph), d(ph.d), f(ph.f), lensRadius(ph.lensRadius), zoom(ph.zoom), 
    sampler(ph.sampler)
{}

Vector3D PinHole::rayDirection(const Point2D& p)const{
    Vector3D dir = p.x*u + p.y*v - d*w;
    dir.normalize();
    return dir;
}

void ThinLens::setSampler(Sampler* sp){
    if(sampler){
        delete sampler;
        sampler = NULL;
    }
    sampler = sp;
    sampler->mapToUnitDisk();
}

Vector3D ThinLens::rayDirection(const Point2D& pixel,const Point2D& lens) const{
    Point2D p;
    p.x = pixel.x * f / d;
    p.y = pixel.y * f / d;
    Vector3D dir = (p.x - lens.x) * u + (p.y - lens.y)*v - f*w;
    dir.normalize();
    return dir;
}

void ThinLens::render(World& w) {
    RGBColor	L;
    Ray			ray;
    ViewPlane	vp(w.vp);
    int 		depth = 0;

    Point2D sp;			// sample point in [0, 1] X [0, 1]
    Point2D pp;			// sample point on a pixel
    Point2D dp; 		// sample point on unit disk
    Point2D lp;			// sample point on lens

    //w.open_window(vp.hres, vp.vres);
    vp.pixelSize /= zoom;

    for (int r = 0; r < vp.vres; r++)			// up
        for (int c = 0; c < vp.hres; c++) {		// across 
            L = black;

            for (int n = 0; n < vp.sampleCount; n++) {
                sp = vp.sampler->sampleUnitSquare();
                pp.x = vp.pixelSize * (c - vp.hres / 2.0 + sp.x);
                pp.y = vp.pixelSize * (r - vp.vres / 2.0 + sp.y);

                dp = sampler->sampleUnitDisk();
                lp = dp * lensRadius;

                ray.o = eye + lp.x * u + lp.y * v;
                ray.d = rayDirection(pp, lp);
                //L += w.tracer_ptr->trace_ray(ray, depth);
                L += w.tracer->traceRay(ray);
            }

            L /= vp.sampleCount;
            L *= exposureTime;
            w.setPixel(r, c, L);
        }
}
FishEye::FishEye()
    : Camera(), psiMax(90)
{
}

FishEye::FishEye(const FishEye& fe)
    : Camera(fe), psiMax(fe.psiMax)
{}
void FishEye::render(World& w){
    RGBColor	L;
    ViewPlane	vp(w.vp);
    int 		hres = vp.hres;
    int 		vres = vp.vres;
    float		s = vp.pixelSize;
    Ray			ray;
    int 		depth = 0;
    Point2D 	sp; 					// sample point in [0, 1] X [0, 1]
    Point2D 	pp;						// sample point on the pixel
    float		r_squared;				// sum of squares of normalised device coordinates

    ray.o = eye;

    for (int r = 0; r < vres; r++)		// up
        for (int c = 0; c < hres; c++) {	// across 					
            L = black;

            for (int j = 0; j < vp.sampleCount; j++) {
                sp = vp.sampler->sampleUnitSquare();
                pp.x = s * (c - 0.5 * hres + sp.x);
                pp.y = s * (r - 0.5 * vres + sp.y);
                ray.d = rayDirection(pp, hres, vres, s, r_squared);

                if (r_squared <= 1.0)
                    L += w.tracer->traceRay(ray);
                //L += wr.tracer_ptr->trace_ray(ray, depth);
            }

            L /= vp.sampleCount;
            L *= exposureTime;
            w.setPixel(r, c, L);
        }
}

Vector3D FishEye::rayDirection(const Point2D& pp, const int hres,
                                const int vres, const float	s, float& r2) const {

    Point2D pn(2.0 / (s * hres) * pp.x, 2.0 / (s * vres) * pp.y);
    r2 = pn.x * pn.x + pn.y * pn.y;

    if (r2 <= 1.0) {
        float r = sqrt(r2);
        float psi = r * psiMax * PI_ON_180;
        float sin_psi = sin(psi);
        float cos_psi = cos(psi);
        float sin_alpha = pn.y / r;
        float cos_alpha = pn.x / r;
        Vector3D dir = sin_psi * cos_alpha * u + sin_psi * sin_alpha * v - cos_psi * w;

        return (dir);
    }
    else
        return (Vector3D(0.0));
}

Vector3D Spherical::rayDirection(const Point2D&	pp, const int hres,
                                  const int	vres, const float s) const {
    Point2D pn(2.0 / (s * hres) * pp.x, 2.0 / (s * vres) * pp.y);

    float lambda = pn.x * lambdaMax * PI_ON_180;
    float psi = pn.y * psiMax * PI_ON_180;
    float phi = PI - lambda;
    float theta = 0.5 * PI - psi;

    float sin_phi = sin(phi);
    float cos_phi = cos(phi);
    float sin_theta = sin(theta);
    float cos_theta = cos(theta);

    Vector3D dir = sin_theta * sin_phi * u + cos_theta * v + sin_theta * cos_phi * w;

    return (dir);
}

void Spherical::render(World& wr) {
    RGBColor	L;
    ViewPlane	vp(wr.vp);
    int 		hres = vp.hres;
    int 		vres = vp.vres;
    float		s = vp.pixelSize;
    Ray			ray;
    int 		depth = 0;
    Point2D 	sp; 					// sample point in [0, 1] X [0, 1]
    Point2D 	pp;						// sample point on the pixel

    ray.o = eye;

    for (int r = 0; r < vres; r++)		// up
        for (int c = 0; c < hres; c++) {	// across 					
            L = black;

            for (int j = 0; j < vp.sampleCount; j++) {
                sp = vp.sampler->sampleUnitSquare();
                pp.x = s * (c - 0.5 * hres + sp.x);
                pp.y = s * (r - 0.5 * vres + sp.y);
                ray.d = rayDirection(pp, hres, vres, s);
                L += wr.tracer->traceRay(ray);
            }

            L /= vp.sampleCount;
            L *= exposureTime;
            wr.setPixel(r, c, L);
        }
}

Spherical::Spherical()
    : Camera(), psiMax(90), lambdaMax(180)
{
}

Spherical::Spherical(const Spherical& s)
    : Camera(s), psiMax(s.psiMax), lambdaMax(s.lambdaMax)
{}


StereoCamera::StereoCamera()
    : Camera(),type(ViewingType::parallel), pixelGap(5), beta(1.0f), 
    leftCamera(NULL),rightCamera(NULL)
{}

StereoCamera::StereoCamera(const StereoCamera& sc)
    : Camera(sc), type(sc.type), pixelGap(sc.pixelGap), beta(sc.beta),
    leftCamera(sc.leftCamera), rightCamera(sc.rightCamera)
{}

void StereoCamera::render(World& w){
    ViewPlane	vp = w.vp;
    int 		hres = vp.hres;
    int 		vres = vp.vres;

    //w.open_window(2 * hres + pixel_gap, vres);

    double r = eye.distance(lookAt);
    double x = r * tan(0.5 * beta * PI_ON_180);

    if (type == parallel) {
        //leftCamera->render_stereo(w, x, 0);						// left view on left
        //rightCamera->render_stereo(w, -x, hres + pixel_gap);   	// right view on right
    }

    if (type == transverse) {
        //right_camera_ptr->render_stereo(w, -x, 0);   					// right view on left
        //left_camera_ptr->render_stereo(w, x, hres + pixel_gap);    		// left view on right
    }
}


void StereoCamera::setupCameras(void){
    double r = eye.distance(lookAt);
    double x = r * tan(0.5 * beta * PI_ON_180);  //  half the camera separation distance

    leftCamera->setEye(eye - (x * u)); // x * u is view coord to world coord 
    leftCamera->setLookAt(lookAt - (x * u));
    leftCamera->computeUVW();

    rightCamera->setEye(eye + (x * u));
    rightCamera->setLookAt(lookAt + (x * u));
    rightCamera->computeUVW();
}