#include "RTLYSampler.h"
#include "RTLYMath.h"
#include <algorithm>
using namespace RTLY;

Sampler::Sampler(){
    sampleCount = 83;
    setCount = 1;
    jump = 0;
    count = 0;
    samples.reserve(sampleCount*setCount);
    setupShuffledIndices();
}

Sampler::Sampler(const int count){
    sampleCount = count;
    setCount = 83;
    jump = 0;
    this->count = 0;
    samples.reserve(sampleCount*setCount);
    setupShuffledIndices();
}

Sampler::Sampler(const int count, const int m){
    sampleCount = count;
    setCount = m;
    jump = 0;
    this->count = 0;
    samples.reserve(sampleCount*setCount);
    setupShuffledIndices();
}

Sampler::Sampler(const Sampler& s){
    if( this == &s)
        return;
    sampleCount = s.sampleCount;
    setCount = s.setCount;
    samples = s.samples;
    shuffledIndices = s.shuffledIndices;
    disk_samples = s.disk_samples;
    sphere_samples = s.sphere_samples;
    hemisphere_samples = s.hemisphere_samples;
    count = s.count;
    jump = s.jump;
}


Sampler& Sampler::operator=(const Sampler& rhs){
    if(this == &rhs){
        return *this;
    }

    sampleCount = rhs.sampleCount;
    setCount = rhs.setCount;
    samples = rhs.samples;
    shuffledIndices = rhs.shuffledIndices;
    disk_samples = rhs.disk_samples;
    sphere_samples = rhs.sphere_samples;
    hemisphere_samples = rhs.hemisphere_samples;
    count = rhs.count;
    jump = rhs.jump;
    return *this;
}

void Sampler::shuffleXCoord(void) {
    for (int p = 0; p < setCount; p++)
        for (int i = 0; i <  sampleCount - 1; i++) {
            int target = rand_int() % sampleCount + p * sampleCount;
            float temp = samples[i + p * sampleCount + 1].x;
            samples[i + p * sampleCount + 1].x = samples[target].x;
            samples[target].x = temp;
        }
}

void Sampler::shuffleYCoord(void) {
    for (int p = 0; p < setCount; p++)
        for (int i = 0; i <  sampleCount - 1; i++) {
            int target = rand_int() % sampleCount + p * sampleCount;
            float temp = samples[i + p * sampleCount + 1].y;
            samples[i + p * sampleCount + 1].y = samples[target].y;
            samples[target].y = temp;
        }	
}


Point2D Sampler::sampleUnitSquare(){
    if(count % sampleCount == 0)
        jump = (rand_int() % setCount) * sampleCount;
    return samples[jump + shuffledIndices[jump+count++%sampleCount]];
}

Point2D Sampler::sampleUnitDisk(void) {
    if (count % sampleCount == 0)  									// start of a new pixel
        jump = (rand_int() % setCount) * sampleCount;
    return (disk_samples[jump + shuffledIndices[jump + count++ % sampleCount]]);
}

Point3D Sampler::sampleHemisphere(void) {
    if (count % sampleCount == 0)  									// start of a new pixel
        jump = (rand_int() % setCount) * sampleCount;
    return (hemisphere_samples[jump + shuffledIndices[jump + count++ % sampleCount]]);		
}

Point3D Sampler::sampleSphere(void) {
    if (count % sampleCount == 0)  									// start of a new pixel
        jump = (rand_int() % setCount) * sampleCount;
    return (sphere_samples[jump + shuffledIndices[jump + count++ % sampleCount]]);		
}

Point2D Sampler::sampleOneSet(void) {
        return(samples[count++ % sampleCount]);  
}

void Sampler::setupShuffledIndices(){
    shuffledIndices.reserve(sampleCount*setCount);
    vector<int> indices;
    for(int i = 0; i < sampleCount; i++)
        indices.push_back(i);
    for(int p = 0; p < setCount; p++){
        random_shuffle(indices.begin(),indices.end());

        for(int j = 0; j< sampleCount; j++)
            shuffledIndices.push_back(indices[j]);
    }
}

void Sampler::mapToUnitDisk(void) {
        int size = samples.size();
        float r, phi;		// polar coordinates
        Point2D sp; 		// sample point on unit disk
        disk_samples.reserve(size);
        Point2D zero;
        for (int i = 0; i < size; i++){
            disk_samples.push_back(zero);
        }
        for (int j = 0; j < size; j++) {
            // map sample point to [-1, 1] X [-1,1]
            sp.x = 2.0 * samples[j].x - 1.0;	
            sp.y = 2.0 * samples[j].y - 1.0;
            if (sp.x > -sp.y) {			// sectors 1 and 2
                if (sp.x > sp.y) {		// sector 1
                    r = sp.x;
                    phi = sp.y / sp.x;
                }
                else {					// sector 2
                    r = sp.y;
                    phi = 2 - sp.x / sp.y;
                }
            }
            else {						// sectors 3 and 4
                if (sp.x < sp.y) {		// sector 3
                    r = -sp.x;
                    phi = 4 + sp.y / sp.x;
                }
                else {					// sector 4
                    r = -sp.y;
                    if (sp.y != 0.0)	// avoid division by zero at origin
                        phi = 6 - sp.x / sp.y;
                    else
                        phi  = 0.0;
                }
            }
            phi *= PI / 4.0;
            disk_samples[j].x = r * cos(phi);
            disk_samples[j].y = r * sin(phi);
        }
        samples.erase(samples.begin(), samples.end());
}

void Sampler::mapToHemisphere(const float exp) {
        int size = samples.size();
        hemisphere_samples.clear();
        hemisphere_samples.reserve(sampleCount * setCount);

        for (int j = 0; j < size; j++) {
            float cos_phi = cos(2.0 * PI * samples[j].x);
            float sin_phi = sin(2.0 * PI * samples[j].x);	
            float cos_theta = pow((1.0 - samples[j].y), 1.0 / (exp + 1.0));
            float sin_theta = sqrt (1.0 - cos_theta * cos_theta);
            float pu = sin_theta * cos_phi;
            float pv = sin_theta * sin_phi;
            float pw = cos_theta;
            hemisphere_samples.push_back(Point3D(pu, pv, pw)); 
        }
}

void Sampler::mapToSphere(void) {
        float r1, r2;
        float x, y, z;
        float r, phi;
        sphere_samples.reserve(sampleCount * setCount);   
        for (int j = 0; j < sampleCount * setCount; j++) {
            r1 	= samples[j].x;
            r2 	= samples[j].y;
            z 	= 1.0 - 2.0 * r1;
            r 	= sqrt(1.0 - z * z);
            phi = TWO_PI * r2;
            x 	= r * cos(phi);
            y 	= r * sin(phi);
            sphere_samples.push_back(Point3D(x, y, z)); 
        }
}



// ---------------------------------------------------------------- constructor

JitteredSampler::JitteredSampler():Sampler(){
    generateSampler();
}

JitteredSampler::JitteredSampler(const int count):Sampler(count){
    generateSampler();
}

JitteredSampler::JitteredSampler(const int count,const int m):Sampler(count,m){
    generateSampler();
}


JitteredSampler& JitteredSampler::operator=(const JitteredSampler& rhs){
    if(this == &rhs)
        return *this;
    Sampler::operator=(rhs);
    return *this;
}
JitteredSampler* JitteredSampler::clone() const{
      return new JitteredSampler(*this);
}

void JitteredSampler::generateSampler(){
    int n = (int) sqrt(sampleCount);

    for(int p = 0; p < setCount ; p++)
        for(int j = 0; j < n ; j++){
            for(int k = 0; k < n; k++){
                Point2D sp((k + rand_float())/n,(j+rand_float())/n);
                samples.push_back(sp);
            }
        }
}

// ---------------------------------------------------------------- constructor

RegularSampler::RegularSampler(void) : Sampler(){
    generateSampler();
}



RegularSampler::RegularSampler(const int num):Sampler(num){
    generateSampler();
}

RegularSampler::RegularSampler(const RegularSampler& u)	: Sampler(u){
    generateSampler();
}

RegularSampler& RegularSampler::operator= (const RegularSampler& rhs)	{
    if (this == &rhs)
        return (*this);
    Sampler::operator= (rhs);
    return (*this);
}

RegularSampler*	RegularSampler::clone(void) const {
        return (new RegularSampler(*this));
}

RegularSampler::~RegularSampler(void) {}

void RegularSampler::generateSampler(void){
    int n = (int) sqrt((float)sampleCount);
    for (int j = 0; j < setCount; j++)
        for (int p = 0; p < n; p++)		
            for (int q = 0; q < n; q++)
                samples.push_back(Point2D((q + 0.5) / n, (p + 0.5) / n));
}

// ---------------------------------------------------------------- constructor

MultiJitteredSampler::MultiJitteredSampler(void)							
	: Sampler()
{}


MultiJitteredSampler::MultiJitteredSampler(const int num_samples)
	: 	Sampler(num_samples) {
//	count = 0;
//	jump = 0;
	generateSampler();
}


MultiJitteredSampler::MultiJitteredSampler(const int num_samples, const int m)
	: 	Sampler(num_samples, m) {
	generateSampler();
}

MultiJitteredSampler::MultiJitteredSampler(const MultiJitteredSampler& mjs)			
	: Sampler(mjs)
{
	generateSampler();
}


MultiJitteredSampler& MultiJitteredSampler::operator= (const MultiJitteredSampler& rhs)	{
	if (this == &rhs)
		return (*this);
		
	Sampler::operator=(rhs);

	return (*this);
}


MultiJitteredSampler* MultiJitteredSampler::clone(void) const {
	return (new MultiJitteredSampler(*this));
}

MultiJitteredSampler::~MultiJitteredSampler(void) {}


void MultiJitteredSampler::generateSampler(void) {		
	// num_samples needs to be a perfect square
			
	int n = (int)sqrt((float)sampleCount);
	float subcell_width = 1.0 / ((float) sampleCount);
	
	Point2D fill_point;
	for (int j = 0; j < sampleCount * setCount; j++)
		samples.push_back(fill_point);
	
	for (int p = 0; p < setCount; p++) 
		for (int i = 0; i < n; i++)		
			for (int j = 0; j < n; j++) {
				samples[i * n + j + p * sampleCount].x = (i * n + j) * subcell_width + rand_float(0, subcell_width);
				samples[i * n + j + p * sampleCount].y = (j * n + i) * subcell_width + rand_float(0, subcell_width);
			}
	
	for (int p = 0; p < setCount; p++) 
		for (int i = 0; i < n; i++)		
			for (int j = 0; j < n; j++) {
				int k = rand_int(j, n - 1);
				float t = samples[i * n + j + p * sampleCount].x;
				samples[i * n + j + p * sampleCount].x = samples[i * n + k + p * sampleCount].x;
				samples[i * n + k + p * sampleCount].x = t;
			}

	for (int p = 0; p < setCount; p++)
		for (int i = 0; i < n; i++)		
			for (int j = 0; j < n; j++) {
				int k = rand_int(j, n - 1);
				float t = samples[j * n + i + p * sampleCount].y;
				samples[j * n + i + p * sampleCount].y = samples[k * n + i + p * sampleCount].y;
				samples[k * n + i + p * sampleCount].y = t;
		}
}


