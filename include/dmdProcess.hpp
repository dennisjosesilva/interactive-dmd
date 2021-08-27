
#include <stdio.h>
#include "connected.hpp"
#include "skeleton_cuda.hpp"


class dmdProcess {
  public:
    
    dmdProcess();
    ~dmdProcess();
    void set_filename (const char *c_str) {filename = c_str;}
    void readImage () 
    { 
      processedImage = FIELD<float>::read(filename); 
      nPix = processedImage->dimX() * processedImage->dimY();
    }
    void removeIslands(float islandThreshold);
    void find_layers(int clear_color, double* importance_upper, double width);
    void calculateImportance(bool cumulative, int num_layers);
    void removeLayers();
    void computeSkeletons();

    int clear_color;
  
  private:
    const char *filename;
    FIELD<float>* processedImage;
    int nPix;
    double *importance;


};

