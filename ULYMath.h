#pragma once
#include "ULYType.h"
#include <string>

namespace ULY{

    u64  cn2      (u64 n);

    f32  invSqrt  (f32 x);              //return 1/sqrt(x) 比标准库快4倍

    void permute_l(i32 n);              //permute in lexical order 

    void permute_r(i32 n);              //permute by rotate

    u64  power    (u64 m, u64 n);       //m^n

    f32  random   (bool reset=false);   //return 0,0x7fff(32768)

    mat4 rotate   (vec4 vec,f32 radian); //axis(a,b,c) rotate theta radian

    void subset   (i32 n);              //All Possible Subsets Generation by Lexical Order {} {1} {1,2}...

    void ksubset  (i32 n, i32 k);       //All Possible K Subsets Generation by Lexical Order {1,2,3}...
    
    void setpart  (i32 n);



#define  min3(a,b,c)  (((a) < (b)) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

    //100000数据量 define 0.000589,template 0.000792
    template<typename T>
    inline T min3_2(T a, T b, T c){
        return a<b?(a<c?a:c):(b<c?b:c);
    }

    template<typename T>
    bool min1st2nd(T data[],int count, T &first, T& second);

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

    template<typename T>
    bool min1st2nd(T data[],int count, T &first, T& second){
        if(count<1)
           return false; 
        if(count == 1){
            first = second = data[0];
            return true;
        }
        if(data[0]<data[1]){
            first = data[0]; second = data[1];
        }else{
            first = data[1]; second = data[0];
        }
        for(int i=2;i<count;i++){
            if(data[i]<first){
                second = first;
                first = data[i];
            }else if(data[i]<second){
                second = data[i];
            }
        }
        return true;
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
