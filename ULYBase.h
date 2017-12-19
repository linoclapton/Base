#pragma once

#include <windows.h>
#include "ULYType.h"
#include "ULY2DIO.h"
#include <iostream>

//取高位低位
#define WORD_HI(xxx) ((byte) ((word)(xxx)>>8))
#define WORD_LO(xxx) ((byte) ((word)(xxx)&255))

//返回array大小
#define ARR_SIZE(a) (sizeof((a))/sizeof((a[0])))

namespace ULY{
using namespace std;

class Clock{
private:
    static const int N = 50;
    int index;
    LARGE_INTEGER startCounter[N],frequency,stopCounter[N];
    string text;
public:
    Clock(){
        QueryPerformanceFrequency(&frequency);
        text = "";
        index = 0;
    }
    Clock(string text):text(text){
        QueryPerformanceFrequency(&frequency);
        index = 0;
    }
    void start(){
        QueryPerformanceCounter(&startCounter[++index]);
    }
    void end(string text){
        QueryPerformanceCounter(&stopCounter[index]);
        printf("%s %f\n",text.c_str(),(double)(stopCounter[index].QuadPart - startCounter[index].QuadPart)/frequency.QuadPart);
        index--;
    }
    void end(){
        QueryPerformanceCounter(&stopCounter[index]);
        printf("%s %f\n",text.c_str(),(double)(stopCounter[index].QuadPart - startCounter[index].QuadPart)/frequency.QuadPart);
        index--;
    }
};
    //draw picture on destop directly
    void drawPixelOnDesktop(Pixel &p);

    int KMP(std::string src, std::string pattern);

    int BM (std::string src, std::string pattern);

    int Sunday(std::string src, std::string pattern);

    int* subSequence(std::string src, std::string pattern);

    void shortestPath(int *dist,int *path, int n);

    void write(string path,char* data,size_t size);
    void read(string path,char* data,size_t size);

}