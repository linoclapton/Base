#include "RTLYColor.h"

using namespace RTLY;

RGBColor& RGBColor::operator= (const RGBColor& rhs){
    if( this == &rhs)
        return (*this);
    r = rhs.r; g = rhs.g; b = rhs.b;
    return (*this);
}

RGBColor RGBColor::powc(float p) const{
    return RGBColor(pow(r,p),pow(g,p),pow(b,p));
}