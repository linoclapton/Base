#include "RTLYWorld.h"
#include "RTLYCamera.h"
#include "RTLYUtility.h"
#include "RTLYTracer.h"
#include "RTLYLight.h"
#include "RTLYMaterial.h"
#include "RTLYMath.h"

using namespace RTLY;

World::World(){
    int samplesCount = 1;
    //vp.hres = 600;
    //vp.vres = 600;
    //vp.pixelSize = 0.5;
    //vp.setSampler(new JitteredSampler(samplesCount));
    //vp.setSampler(new RegularSampler(samplesCount));
    //vp.sampler->mapToHemisphere(10);
    //background = white;
    //imageWidth = vp.hres;
    //imageHeight = vp.vres;
    //imageBuffer = new RGBColor[imageHeight*imageWidth];
    //tracer = new RayCast(this);

}
void World::build(){
    int num_samples = 256;   		// for Figure 18.4(a)
    //	int num_samples = 100;   	// for Figure 18.4(b) & (c)

    Sampler* sampler_ptr = new MultiJitteredSampler(num_samples);
    vp.maxDepth = 0;
    vp.hres = 400;
    vp.vres = 400;
    vp.sampleCount = num_samples;
    vp.sampler = sampler_ptr;	

    background = RGBColor(0.5);
    imageWidth = vp.hres;
    imageHeight = vp.vres;
    imageBuffer = new RGBColor[imageHeight*imageWidth];

    tracer = new AreaLighting(this);

    PinHole* camera = new PinHole;
    camera->setEye(25, 200, 100);
    camera->setLookAt(-0.5, 0, 0); 	
    camera->setDistance(8000);          
    camera->computeUVW();     
    this->camera = camera; 

    Ambient* ambient_ptr = new Ambient;
    ambient_ptr->scaleRadiance(0.0);
    ambient_ptr->setColor(0.0);
    ambient_ptr->setShadow(false);
    ambient = ambient_ptr;


    PointLight* light_ptr1 = new PointLight;
    light_ptr1->setLocation(1, 5, 0);
    light_ptr1->scaleRadiance(3.0);
    light_ptr1->setShadow(true);
    addLight(light_ptr1);


    // yellow triangle

    Matte* matte_ptr1 = new Matte;			
    matte_ptr1->setKa(0.25); 
    matte_ptr1->setKd(0.75);
    matte_ptr1->setCd(1, 1, 0);

    Triangle* triangle_ptr1 = new Triangle(Point3D(2, 0.5, 5), Point3D(2, 1.5, -5), Point3D(-1, 0, -4)); 
    triangle_ptr1->set_material(matte_ptr1);
    add_object(triangle_ptr1);


    // dark green triangle (transformed)

    Matte* matte_ptr2 = new Matte;			
    matte_ptr2->setKa(0.25); 
    matte_ptr2->setKd(0.75);
    matte_ptr2->setCd(0.0, 0.5, 0.41);

    Instance* triangle_ptr2 = new Instance(new Triangle(Point3D(2, 1, 5), Point3D(2, 0.5, -5), Point3D(-1, -1, -4))); 
    triangle_ptr2->rotate_y(120);
    triangle_ptr2->set_material(matte_ptr2);
    add_object(triangle_ptr2);


    // brown triangle (transformed)

    Matte* matte_ptr3 = new Matte;			
    matte_ptr3->setKa(0.25); 
    matte_ptr3->setKd(0.75);
    matte_ptr3->setCd(0.71, 0.40, 0.16);

    Instance* triangle_ptr3 = new Instance(new Triangle(Point3D(2, 0, 5), Point3D(2, 1, -5), Point3D(-1, 0, -4))); 
    triangle_ptr3->rotate_y(240);
    triangle_ptr3->set_material(matte_ptr3);
    add_object(triangle_ptr3);

}

void World::setPixel(int r,int c,RGBColor L){
    imageBuffer[r*imageWidth+c] = maxToOne(L);
}
void World::renderScene(){
    RGBColor pixelColor;
    Ray ray;
    double zw = 100.0;
    double x,y;
    Point2D sp;
    Point2D pp;
    ray.d = Vector3D(0,0,-1);
    for(int r=0;r<imageHeight;r++)
    {
        for(int c=0;c<imageWidth;c++){
            pixelColor = black;
            for(int j=0;j<vp.sampleCount;j++){
                sp = vp.sampler->sampleUnitSquare();
                pp.x = c - 0.5*imageWidth + sp.x ;
                pp.y = r - 0.5*imageHeight + sp.y;
                ray.o = Point3D(pp.x,pp.y,zw);
                pixelColor += tracer->traceRay(ray);
            }
            pixelColor /= vp.sampleCount;
            imageBuffer[r*imageWidth+c] = pixelColor;
        }
    }
}
bool World::outputPPMImage(std::string filePath)
{
    FILE *fp = fopen(filePath.c_str(), "wb");
    if (!fp) return false;
    //fprintf(fp, "P6\n%d %d\n255\n", vp.hres, vp.vres);
    fprintf(fp, "P6\n%d %d\n255\n", imageWidth, imageHeight);
    int size = imageWidth * imageHeight;
    // now write out image in binary
    for (int i = imageHeight-1; i >= 0; i--){
        for (int j=0; j < imageWidth; j++){
            int index = i*imageWidth+j;
            unsigned char d[3] = { imageBuffer[index].r * 255, imageBuffer[index].g * 255, imageBuffer[index].b * 255 };
            fwrite(d, 1, 3, fp);
        }
    }
    fclose(fp);
    return true;
}

ShadeRec World::hitObjects(const Ray& ray){
    ShadeRec sr(*this);
    double t;
    Normal normal;
    Point3D localHitPoint;
    double tmin = kHugeValue;
    int numOfObjects = objects.size();

    for(GeoObject* object: objects){
        if(object->hit(ray,t,sr) && t < tmin){
            sr.hitAnObject = true;
            tmin = t;
            sr.material = object->getMaterial();
            sr.hitPoint = ray.o + t*ray.d;
            normal = sr.normal;
            localHitPoint = sr.localHitPoint;
        }
    }
    if(sr.hitAnObject){
        sr.t = tmin;
        sr.normal = normal;
        sr.localHitPoint = localHitPoint;
    }

    return sr;
}