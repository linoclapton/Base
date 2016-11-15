#pragma once

#include <iostream>
#include "RTLYUtility.h"
#include "RTLYGeoObjects.h"
#include "RTLYViewPlane.h"
#include <vector>


namespace RTLY{
    class Tracer;
    class Camera;
    class Light;
    class World{
    public:
        RGBColor background;
        ViewPlane vp;
        Sphere sphere;
        Light* ambient;
        Camera* camera;
        Tracer* tracer;
        std::vector<GeoObject*> objects;
        std::vector<Light*> lights;

        int imageWidth;
        int imageHeight;
        RGBColor* imageBuffer;

        World();
        void build();
        void renderScene();
        //void openWindow(const int hres,const int vres) const;
        //void displayPixel(const int row, const int column, const RGBColor& pixelColor)const;
        bool outputPPMImage(std::string filePath);

        inline void addObject(GeoObject* object){
             objects.push_back(object);
        }
        void addLight(Light* light){
            lights.push_back(light);
        }
        ShadeRec hitObjects(const Ray& ray);
        void setPixel(int r,int c,RGBColor L);
        RGBColor maxToOne(const RGBColor& c)const{
            float max = std::max(c.r,std::max(c.g,c.b));
            if(max>1.0)
                return c/max;
            else
                return c;
        }
        RGBColor clampToColor(const RGBColor& color) const{
            RGBColor c(color);
            if( (color.r>1.0)||color.g>1.0||color.b>1.0){
                c.r = 1.0; c.g=0.0; c.b = 0.0;
            }
            return c;
        }
    };
}
