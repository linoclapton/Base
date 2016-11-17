#pragma once
#include "RTLYConstant.h"

namespace RTLY{

int rand_int();
int rand_int(int l,int h);
float rand_float();
float rand_float(int l, float h);

inline double max(double x0, double x1) {
        return ((x0 > x1) ? x0 : x1);
}

inline double min(double x0, double x1) {
        return ((x0 < x1) ? x0 : x1);
}

inline void set_rand_seed(const int seed) {
        srand(seed);
}

inline int rand_int(void) {
    return(rand());
}

inline float rand_float(int l, float h) {
    return (rand_float() * (h - l) + l);
}

inline int rand_int(int l, int h) {
    return ((int) (rand_float(0, h - l + 1) + l));
}

inline float rand_float() {
    return((float)rand_int() * invRAND_MAX);
}

}
