#pragma once
#include "RTLYUtility.h"
#include <vector>

namespace RTLY{
    using namespace std;
    class Sampler{
    public:
        Sampler();
        Sampler(const int count);
        Sampler(const int count, const int m);
        Sampler(const Sampler& s);
        virtual Sampler* clone() const = 0;

        Sampler& operator=(const Sampler& rhs);

        virtual void generateSampler() = 0;
        void setupShuffledIndices();
        void shuffleXCoord();
        void shuffleYCoord();
        Point2D sampleUnitSquare();
        Point2D sampleUnitDisk();
        Point3D sampleHemisphere(); 
        Point3D sampleSphere();
        Point2D sampleOneSet();
        void mapToUnitDisk();
        void mapToHemisphere(const float exp);
        void mapToSphere();
        int getSampleCount(){return sampleCount;}
    protected:
        int sampleCount;
        int setCount;
        vector<Point2D> samples;
        vector<Point2D>	disk_samples;
        vector<Point3D>	hemisphere_samples;
        vector<int> shuffledIndices;
        vector<Point3D> sphere_samples;
        unsigned long count;
        int jump;
    };

    class JitteredSampler : public Sampler{
    public:
        JitteredSampler();
        JitteredSampler(const int count);
        JitteredSampler(const int count, const int m);
        JitteredSampler(const JitteredSampler& s):Sampler(s){}

        JitteredSampler& operator=(const JitteredSampler& rhs);

        virtual JitteredSampler* clone() const;
        virtual ~JitteredSampler(){}
    private:
        virtual void generateSampler();
    };

    class RegularSampler : public Sampler{
    public:
        RegularSampler();
        RegularSampler(const int count);
        RegularSampler(const int count, const int m);
        RegularSampler(const RegularSampler& s);

        RegularSampler& operator=(const RegularSampler& rhs);

        virtual RegularSampler* clone() const;
        virtual ~RegularSampler();
    private:
        virtual void generateSampler();
    };

    class MultiJitteredSampler: public Sampler {
    public:
        MultiJitteredSampler(void);								
        MultiJitteredSampler(const int sampleCount);				
        MultiJitteredSampler(const int sampleCount, const int m);	
        MultiJitteredSampler(const MultiJitteredSampler& mjs);			
        virtual ~MultiJitteredSampler(void);								

        MultiJitteredSampler& operator= (const MultiJitteredSampler& rhs);				
        virtual MultiJitteredSampler* clone(void) const;			
    private:
        virtual void generateSampler(void);		
    };
}