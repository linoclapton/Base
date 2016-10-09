#include "ULYBase.h"
namespace ULY{

    void drawPixelOnDesktop(Pixel &p){
        HDC hScrDC ; 
        hScrDC =CreateDC(L"DISPLAY",NULL,NULL,NULL);

        for(int i=0;i<p.width;i++)
            for(int j=0;j<p.height;j++){
                int index = (j*p.width+i)*3;
                unsigned pixel = RGB(p.img[index+2],p.img[index+1],p.img[index]);
                SetPixelV(hScrDC,i,p.height-j,pixel);
            }
    }

}
