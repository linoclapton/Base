#pragma once
#include <iostream>
#include <cstdio>
#include <string>
#include <fstream>
#include "ULYTypeDef.h"

namespace ULY{


typedef struct{
    byte transmission;
    byte pngName[3];
    byte dosEnding[2];
    byte dosStop;
    byte unixLineEnding;
} PNG_Header;

//bmp pixel-array store upside down lower-left from left to right then bottom to top
typedef struct{
    byte name[2];
    byte size[4];//total img size include header info image body
    byte reserved1[2];
    byte reserved2[2];
    byte offset[4];
} BMP_HEADER;

typedef struct{
    int infoSize;
    int width;
    int height;
    word colorPlanes;
    word bitsOfPixel;
    int compressionMethod;
    int imageSize;
    int hR;//horizontal Resolution
    int vR;
    int numberOfColors;//color palette 2^n
    int numberOfImportant;//0 every one is important;
} BMP_INFO;

using namespace std;
class Pixel{
public: 
    char* img;
    int height;
    int width;
    Color_Type type;
    BMP_HEADER header;
    BMP_INFO info;
    void readFile(string filename);
    void readBMP(string filename);
    void writeBMP(string filename);
};

}

