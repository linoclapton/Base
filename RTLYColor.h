#pragma once
#include <cmath>
namespace RTLY{

    class RGBColor {
    public:
        float	r, g, b;									

        RGBColor(float c=0.0):r(c),g(c),b(c){}
        RGBColor(float r, float g, float b):r(r),g(g),b(b){}
        RGBColor(const RGBColor& c):r(c.r),g(c.g),b(c.b){}

        ~RGBColor(void){}

        RGBColor& operator= (const RGBColor& rhs); 
        RGBColor  operator+ (const RGBColor& c) const{
            return (RGBColor(r + c.r, g + c.g, b + c.b));
        }
        RGBColor& operator+= (const RGBColor& c){
            r += c.r; g += c.g; b += c.b;
            return (*this);
        }
        RGBColor  operator* (const float a) const{
            return (RGBColor (r * a, g * a, b * a));	
        }
        RGBColor& operator*= (const float a){
            r *= a; g *= a; b *= a;
            return (*this);
        }
        RGBColor operator/ (const float a) const{
            return (RGBColor (r / a, g / a, b / a));
        }
        RGBColor& operator/= (const float a){
            r /= a; g /= a; b /= a;
            return (*this);
        }
        RGBColor operator* (const RGBColor& c) const{
            return (RGBColor (r * c.r, g * c.g, b * c.b));
        }
        bool operator== (const RGBColor& c) const{
            return r==c.r && g==c.g && b==c.b;
        }
        RGBColor powc(float p) const;
        float average(void) const;
        friend RGBColor operator* (const float a, const RGBColor& c);
    };



    inline float RGBColor::average(void) const {
            return (0.333333333333 * (r + g + b));
    }


    inline RGBColor operator* (const float a, const RGBColor& c) {
            return (RGBColor (a * c.r, a * c.g, a * c.b));	
    }
}
