#pragma once
#include <cstdio>
#include <cstring>
#include <cmath>
namespace ULY{
    class BitTools{
    public:
        template<typename T>
        static void show_bytes(T data, bool binary=false, bool humanRead=false);

        static bool tadd_ok (int a, int b);
        static bool tadd_ok2(int a, int b);
        static bool tmul_ok (int a, int b);
        static int  bitAnd  (int x, int y);
        static int  getByte (int x, int n);
        static int  bitCount(int x);
        static int  logicalShift(int x, int n);
        static int  bang(int x);
        static int  fitsBits(int x, int n);
        static int  divpwr2(int x, int n);
        static int  isPositive(int x);
        static int  isLessOrEqual(int x, int y);
        static int  ilog2(int x);
        static unsigned float_neg(unsigned uf);
        static unsigned float_i2f(int x);
        static unsigned float_twice(unsigned uf);
    };

    class BitVector 
    {
    public:
      /* Create a new bit vector with n indices. All bit values will be
         initialized to clear.
      */
      BitVector(int n)
      {
        pvals = new unsigned char[(int)ceil((float)n/8)];
        nvals = n;
        memset(pvals, 0, (int)ceil((float)n/8));
      }

      virtual ~BitVector()
      {
        delete pvals;
      }

      /* Query the value of a bit. Returns 0 if it's not set, non-zero if
         it's currently set.  
      */
      inline int bit_value(int idx)
      {
        unsigned char block = pvals[idx / 8];
        return (block & (0x80u >> (idx % 8)));
      }

      /* Set the bit at the index, regardless of it's current value.  
       */
      inline void bit_set(int idx)
      {
        int blockidx = idx / 8;
        
        pvals[blockidx] = pvals[blockidx] | (0x80u >> (idx % 8));
      }

      /* Clear the bit at the index, regardless of it's current value.
       */
      inline void bit_clear(int idx)
      {
        int blockidx = idx / 8;
        
        pvals[blockidx] = pvals[blockidx] & (~(0x80u >> (idx % 8)));
      }

      /* Reset the vector to it's initial, cleared state
       */
      void reset()
      {
        memset(pvals, 0, (int)ceil((float)nvals/8));
      }

    private:
      unsigned char * pvals;
      int nvals;
    };

}

template<typename T>
void ULY::BitTools::show_bytes(T data,bool binary,bool humanRead){
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
