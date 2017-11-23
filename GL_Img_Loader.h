#pragma once
#include <string>
#include <soil.h>

namespace ULY{
    using namespace std;
    class Image{
    public:
        static unsigned char* load(string path,int* w,int* h,int *channel){
            //unsigned char* img = SOIL_load_image(path.c_str(),w,h,0,SOIL_LOAD_RGB);
            unsigned char* img = SOIL_load_image(path.c_str(),w,h,channel,SOIL_LOAD_RGB);
            return img;
        }
    };
}