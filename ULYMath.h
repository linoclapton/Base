#pragma once
#include "ULYType.h"

namespace ULY{

    u64  cn2      (u64 n);

    f32  invSqrt  (f32 x);              //return 1/sqrt(x) 比标准库快4倍

    void permute_l(i32 n);              //permute in lexical order 

    void permute_r(i32 n);              //permute by rotate

    u64  power    (u64 m, u64 n);       //m^n

    f32  random   (bool reset=false);

    mat4 rotate   (vec4 vec,f32 radian); //axis(a,b,c) rotate theta radian

    void subset   (i32 n);              //All Possible Subsets Generation by Lexical Order {} {1} {1,2}...

    void ksubset  (i32 n, i32 k);       //All Possible K Subsets Generation by Lexical Order {1,2,3}...
    
    void setpart  (i32 n);

    template<typename T>
    T clamp(T value,T min,T max){
        if(value<min)
            return min;
        if(value>max)
            return max;
        return value;
    }

    template<typename T>
    T power(T t,int n){
        if(n>0){
            T tmp = power(t,n>>1);
            return n%2==1?t*tmp*tmp:tmp*tmp;
        }else{
            return 1.0;
        }
    }
}


    //n 比目前ULY中的power慢两倍
    /*int power(int p,unsigned n){
        if(n<0||(p==0&&n==0)) return 0;
        if(n==0) return 1;
        int result = 1;
        int i = 0;
        while(i<32)
            result = ((n<<(i++))&0x80000000)<0?result*result*p:result*result;
        
        return result;
    }*/
