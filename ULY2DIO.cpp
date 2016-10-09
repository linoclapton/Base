#include "ULY2DIO.h"

namespace ULY{

void Pixel::readFile(string filename){
    int length = filename.size();
    if(filename.substr(length-3,3)=="bmp")
        readBMP(filename);
}

void Pixel::readBMP(string filename){
    memset(&header,0,sizeof(BMP_HEADER));
    memset(&info,0,sizeof(BMP_INFO));
    ifstream fin(filename,ios::binary);
    fin.seekg(0,fin.end);
    int length = fin.tellg();
    fin.seekg(0,fin.beg);
    char* data = new char[length];
    fin.read(data,length);
    int offset = 0;
    memcpy((char*)&header,data+offset,sizeof(BMP_HEADER));
    offset += sizeof(BMP_HEADER);
    memcpy((char*)&info,data+offset,sizeof(BMP_INFO));
    offset += sizeof(BMP_INFO);
    width = info.width;
    height = info.height;
    int padding = ((info.bitsOfPixel*info.width+31)/32)*4;
    img = new char[width*height*info.bitsOfPixel/8];
    for(int i=0;i<height;i++)
        memcpy(&img[i*info.bitsOfPixel/8*width],&data[offset+i*padding],info.bitsOfPixel/8*width);
    fin.close();
    delete[] data;
}

void Pixel::writeBMP(string filename){
    ofstream fout(filename,ios::binary);
    fout.write((char*)&header,sizeof(BMP_HEADER));
    fout.write((char*)&info,sizeof(BMP_INFO));
    int padding = ((info.bitsOfPixel*info.width+31)/32)*4;
    char* data = new char[padding];
    memset(data,0,padding);
    for(int i=0;i<height;i++){
        memcpy(data,&img[i*info.bitsOfPixel/8*width],info.bitsOfPixel/8*width);
        fout.write(data,padding);
    }
    fout.close();
    delete[] data;
}

}