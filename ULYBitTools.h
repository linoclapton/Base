#pragma once
#include <cstdio>
namespace ULY{
    template<typename T>
    void show_bytes(T data,bool binary=false,bool humanRead=false){
        int len = sizeof(T);
        unsigned char* pointer = (unsigned char*)&data;
        printf("hex:");
        if(humanRead){
            if(binary){
                for(int i=0;i<len;i++){
                    for(int j=7;j>-1;j--)
                    printf("%d",(pointer[len-i-1]>>j)&1);
                    printf(" ");
                }
            }
            else{
                for(int i=0;i<len;i++)
                    printf(" %.2x",pointer[len-i-1]);
            }
        }else{
            if(binary){
                for(int i=0;i<len;i++){
                    for(int j=7;j>-1;j--)
                    printf("%d",(pointer[i]>>j)&1);
                    printf(" ");
                }
            }
            else{
                for(int i=0;i<len;i++)
                    printf(" %.2x",pointer[i]);
            }
        }
        printf("\n");
    }
    //t means two's-complement 
    bool tadd_ok(int a,int b){
        int sum = a + b;
        bool neg_over = a<0 && b<0 && 0<=sum;
        bool pos_over = 0<=a && 0<=b && sum<0;
        return !neg_over&&!pos_over;
    }

    bool tmul_ok(int a,int b){
        int p = a*b;
        return !a || p/a == b;
    }
    int bitAnd(int x, int y) {
         return  ~((~x)|(~y));
    }
    int getByte(int x, int n) {
        return  (x>>(n<<3))&0xFF;
    }
    int logicalShift(int x, int n) {
        return x>>n&((~0xFF+0xFF)^((0x01&(!!n))<<31)>>(n+~1+1+!n));
    }
    int bitCount(int x) {
        int cF = 0x0F0F0F0F;
        int c5 = 0x55555555;
        int c3 = 0x33333333;
        x = x+(~((x>>1)&c5)+1);
        x = (x&c3) + ((x>>2)&c3);
        x = ((x>>4) + x)&cF;
        x = (x>>8) + x;
        x = (x>>16) + x;
        return x&0xFF; 
    }
    //!x
    int bang(int x) {
        return (((~x+1) | x )>> 31) + 0x01;
    }
    /* 
     * fitsBits - return 1 if x can be represented as an 
     *  n-bit, two's complement integer.
     *   1 <= n <= 32
     *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
     *   Legal ops: ! ~ & ^ | + << >>
     *   Max ops: 15
     *   Rating: 2
     */
    int fitsBits(int x, int n) {
      int n1 = n+~1+1;
      return ((!((x>>n1>>1)+1))&((x>>n1)&1))|!(x>>n1);
    }
    /* 
     * divpwr2 - Compute x/(2^n), for 0 <= n <= 30
     *  Round toward zero
     *   Examples: divpwr2(15,1) = 7, divpwr2(-33,4) = -2
     *   Legal ops: ! ~ & ^ | + << >>
     *   Max ops: 15
     *   Rating: 2
     */
    int divpwr2(int x, int n) {
        int k = (x>>31) & 1;
        return (x + (k<<n) + (~(1&k) + 1 ))>>n;
    }
    int isPositive(int x) {
        return  !((x>>31) & 1) & !!x;
    }
    int isLessOrEqual(int x, int y) {
        int r = y + (~x+1);
        return  (!((!(x>>31)&1) & (y>>31)&1)) & (((x>>31)&1 & !((y>>31)&1)) | !((r>>31) & 1))  ;
    }
    /*
     * ilog2 - return floor(log base 2 of x), where x > 0
     *   Example: ilog2(16) = 4
     *   Legal ops: ! ~ & ^ | + << >>
     *   Max ops: 90
     *   Rating: 4
     */
    int ilog2(int x) {
      int c = 0;
      c = (!!(x>>16))<<4;
      c = c + ((!!(x>>(c+8)))<<3);
      c = c + ((!!(x>>(c+4)))<<2);
      c = c + ((!!(x>>(c+2)))<<1);
      c = c + (!!(x>>(c+1)));
      return c;
    }
    //return -uf
    unsigned float_neg(unsigned uf) {
        unsigned nx = uf + (0x1<<31); 
        unsigned nf = 0xFF<<23;
        unsigned ux = nx&uf;
        if(!((ux>>23) ^ 0xFF)&&(ux ^ nf)){
            return uf; 
        }else
            return nx; 
    }
    // return x in float format
    unsigned float_i2f(int x) {
        unsigned shiftLeft=0;  
        unsigned afterShift, tmp, flag;  
        unsigned absX=x;  
        unsigned sign=0;  
        if (x==0) return 0;  
        if (x<0)  
        {  
            sign=0x80000000;  
            absX=-x;  
        }  
        afterShift=absX;  
        while (1)  
        {  
            tmp=afterShift;  
            afterShift<<=1;  
            shiftLeft++;  
            if (tmp & 0x80000000) break;  
        }  
        if ((afterShift & 0x01ff)>0x0100)  
            flag=1;  
        else if ((afterShift & 0x03ff)==0x0300)  
            flag=1;  
        else  
            flag=0;  
        return sign + (afterShift>>9) + ((159-shiftLeft)<<23) + flag;
    }
    //return 2*uf in float format use unsigned bytes
    unsigned float_twice(unsigned uf) {
        unsigned f=uf;  
        if ((f & 0x7F800000) == 0){  
            f = ((f & 0x007FFFFF)<<1) | (0x80000000 & f);  
        }  
        else if ((f & 0x7F800000) != 0x7F800000){  
            f =f+0x00800000;  
        }  
        return f;  
    }
}