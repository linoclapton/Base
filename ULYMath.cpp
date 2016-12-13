#include "ULYMath.h"
#include <ctime>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <cstring>

using namespace ULY;

//return max index ordered pair(first,second) first index
int maxPair(int *P,int n){
    for(int i=n-2;i>-1;i--){
        if(P[i]<P[i+1])
            return i;
    }
    return -1;
}
//flip [left,right)
void flip(int *P,int left,int right){
    for(int i=left,j=right-1;i<j;i++,j--){
        std::swap(P[i],P[j]);
    }
}

//return 1/sqrt(x) 比标准库快4倍
float ULY::invSqrt(float x){
    float xhalf = 0.5f*x;
    int i = *(int*)&x; // get bits for floating VALUE 
    i = 0x5f375a86 - (i>>1); // gives initial guess y0
    x = *(float*)&i; // convert bits BACK to float
    x = x*(1.5f-xhalf*x*x); // Newton step, repeating increases accuracy
    return x;
}

// 从右侧找到连续最大逆序列，交换边界，倒序
// j = max{i|P[i]<P[i+1]} 
// k = max{i|P[i]>P[j]}
// swap(P[j],P[k])
// flip(P[j+1],P[n])
void ULY::permute_l(int n){
    int* P = new int[n];
    for(int i=0;i<n;i++)
        P[i] = i+1;
    int j,k;
    for(int i=0;i<n;i++)
        std::cout<<P[i];
    std::cout<<std::endl;
    while((j=maxPair(P,n))!=-1){
        for(int i=n-1;i>j;i--)
            if(P[i] > P[j]){
                k=i;break;
            }
        std::swap(P[j],P[k]);
        flip(P,j+1,n);
        for(int i=0;i<n;i++)
            std::cout<<P[i];
        std::cout<<std::endl;
    }
    delete[] P;
}

void ULY::permute_r(int n){

    int *perm = new int[n];
    int  position;
    int  temp;             

    for (int i = 0; i < n; i++)  /* initialize to 1,2,...,n  */
        perm[i] = i + 1;

    position = n - 1;           
    while (position != 0) {  /* if still have positions..*/
        printf("\n");       /* display result           */
        for (int i = 0; i < n; i++)
            printf("%d ", perm[i]);
        position = n - 1;   /* starts from the last pos */
        temp = perm[position];            
        for (int i = position-1; i >= 0; i--) 
            perm[i+1] = perm[i];  
            perm[0] = temp;            
        while (perm[position]==position+1 && position!=0) {
            position--;    /* if last pos are equal and*/
            temp = perm[position];            
            for (int i = position-1; i >= 0; i--) 
                perm[i+1] = perm[i];  
                perm[0] = temp;            
        }
    }
    delete[] perm;

}

unsigned long ULY::power(unsigned long m, unsigned long n)
{
    unsigned long  temp = 1;

    while (n > 0) {         
        if (n & 0x01UL == 1)
            temp *= m;     
        m *= m;             
        n >>= 1;            
    }
    return temp;
}

float ULY::random(bool reset){
    static bool first = true;
    if(first||reset){
        srand (time(NULL)); 
        first = false;
    }
    return rand();
}

mat4 ULY::rotate(vec4 vec,float radian){
    mat4 m;
    m = new vec4[4];
    float a2 = vec[0]*vec[0], b2 = vec[1]*vec[1], c2 = vec[2]*vec[2],
          ab = vec[0]*vec[1], ac = vec[0]*vec[2], bc = vec[1]*vec[2];
    float c  = std::cos(radian) , s = std::sin(radian);
    m[0][0] = a2+(1-a2)*c;m[0][1] = ab*(1-c)-vec[2]*s;m[0][2]=ac*(1-c)+vec[1]*s;m[0][3]=0.0f;
    m[1][0] = ab*(1-c)+vec[2]*s;m[1][1] = b2+(1-b2)*c ;m[1][2]=bc*(1-c)-vec[0]*s;m[1][3]=0.0f;
    m[2][0] = ac*(1-c)-vec[1]*s;m[2][1] = bc*(1-c)+vec[0]*s;m[2][2]=c2+(1-c2)*c;m[2][3]=0.0f;
    m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
    return m;
}

unsigned long  ULY::cn2(unsigned long n)
{
    unsigned long  x    = (1 << n) + 1;  // 2^n + 1         
    unsigned long  mask = (1 << n) - 1;  // 2^n - 1, n 1's  

    return (ULY::power(x, n) >> ((n >> 1)*n)) & mask;
}


void ULY::subset(int n){
    int* set = new int[n];
    memset(set,0,n);
    printf("\n{}");          /* the empty set            */
    int position  = 0;       /* start from the 1st pos.  */
    set[position] = 1;       /* it gets a '1'            */
    while (true) {           /* loop until done...       */
        printf("\n{%d", set[0]);  /* print one result   */
        for (int i = 1; i <= position; i++)
            printf(",%d", set[i]);
        printf("}");

        if (set[position] < n) { /* this pos. can be inc*/
            set[position+1] = set[position] + 1; /* YES*/
            position++;    /* inc. next pos.           */
        }
        else if (position != 0)  /* NO, the 1st pos?    */
            set[--position]++;  /* backup and increase */
        else                /* NO, the 1st pos and can  */
            break;         /* not be inc. JOB DONE!    */
    }
    delete[] set;
}
// s[k] != n s[k]++
// s[k] == n p == k-1 s[p]++ i = p...k  s[i+1] = s[i] + 1
void ULY::ksubset(int n,int k){

    int* set = new int[n];
    int i,j,position;
    for (i = 0; i < k; i++)  /* initialize the subset to */
        set[i] = i + 1;     /* {1,2,3,...,k}            */
    printf("\n{%d", set[0]); /* display it               */
    for (j = 1; j < k; j++)
        printf(",%d", set[j]);
    printf("}");

    position = k - 1;         /* 'position' indicates the */
    while (true) {            /* pos to be increased      */
        if (set[k-1] == n)    /* if last pos is full, then*/
            position--;       /* move position left       */
        else                  /* or if not full, then the */
            position = k - 1; /* pos is always the last*/
        set[position]++;      /* so add one to the pos.   */
        for (i = position+1; i < k; i++) /* add inc. all*/
            set[i] = set[i-1] + 1; /* pos to its right */

        printf("\n{%d", set[0]); /* display it.         */
        for (j = 1; j < k; j++)
            printf(",%d", set[j]);
        printf("}");

        if (set[0] >= n-k+1) break; /* if 1st pos full  */
    }
}

// S[n] : 1 -> max{S[i] | i < n} + 1
// while S[k] == max{S[i] | i < n} + 1 : S[k] = 1, k--; 
void ULY::setpart(i32 n){
    int* s = new int[n];
    int* max = new int[n];
    for(int i=0;i<n;i++){
        s[i] = 1;
        max[i] = 1;
    }
    for(int i=0;i<n;i++)
        printf("%d ",s[i]);
    printf("\n");
    while(s[n-1]!=n){
        int k = n-1;
        if(s[k]<max[k]+1){
            s[k]++;
        }else{
            s[k]=1;
            k--;
            while(s[k]==max[k]+1){
                s[k--] = 1;
            }
            s[k]++;
            for(int i=k+1;i<n;i++)
                max[i] = s[k];
        }
        for(int i=0;i<n;i++)
            printf("%d ",s[i]);
        printf("\n");
    }
    delete[] max;
    delete[] s;

}