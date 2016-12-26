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

    int KMP(std::string src, std::string p){
        int sl = src.length();
        int pl = p.length();
        int *next = new int[pl];
        int i = 0, j = 0, k = -1;
        next[0] = -1;
        //计算next数组
        while( j<pl ){
            if( k == -1 || p[j] == p[k] ){
                k++;
                j++;
                next[j] = k;
            }
            else
                k = next[k];
        }
        i = 2;
        //优化next数组
        while(j<pl){
            k = next[i];
            if( k==-1 || p[j]==p[k] ){
                next[j] = next[k];
            }
            j++;
        }
        i = 0;
        j = 0;
        //匹配
        while(i<sl&&j<pl){
            if(j == -1 || src[i]==p[j]){
                i++;
                j++;
            }else{
                j = next[j];
            }
        }
        delete[] next;
        return i-pl;
    }

    int BM(std::string src, std::string p){
        int table[256];
        int sl = src.length();
        int pl = p.length();
        for(int i=0;i<256;i++)
            table[i] = pl;
        for(int i=0;i<pl-1;i++)
            table[p[i]] = pl - i - 1;

        int j,k;
        for(int i=pl-1;i<sl;){
            for(j=pl-1,k=i; j>-1 && src[k] == p[j] ; ){
                j--;
                k--;
            }
            if(j==-1)
                return k+1;
            i = i + table[src[i]];
        }
        return -1;
    }

}
