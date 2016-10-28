#pragma once
#include "ULYTypeDef.h"

namespace ULY{
    //return 1/sqrt(x) 比标准库快4倍
    float invSqrt(float x){
        float xhalf = 0.5f*x;
        int i = *(int*)&x; // get bits for floating VALUE 
        i = 0x5f375a86 - (i>>1); // gives initial guess y0
        x = *(float*)&i; // convert bits BACK to float
        x = x*(1.5f-xhalf*x*x); // Newton step, repeating increases accuracy
        return x;
    }
    //rotate with a specific axis
    //axis(a,b,c) rotate \theta degree
    mat4 rotate(vec4 vec,float radian){
        mat4 m;
        m = new vec4[4];
        float a2 = vec[0]*vec[0], b2 = vec[1]*vec[1], c2 = vec[2]*vec[2],
              ab = vec[0]*vec[1], ac = vec[0]*vec[2], bc = vec[1]*vec[2];
        float c  = cos(radian) , s = sin(radian);
        m[0][0] = a2+(1-a2)*c;m[0][1] = ab*(1-c)-vec[2]*s;m[0][2]=ac*(1-c)+vec[1]*s;m[0][3]=0.0f;
        m[1][0] = ab*(1-c)+vec[2]*s;m[1][1] = b2+(1-b2)*c ;m[1][2]=bc*(1-c)-vec[0]*s;m[1][3]=0.0f;
        m[2][0] = ac*(1-c)-vec[1]*s;m[2][1] = bc*(1-c)+vec[0]*s;m[2][2]=c2+(1-c2)*c;m[2][3]=0.0f;
        m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
        return m;
    }
}