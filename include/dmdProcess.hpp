#pragma once

#include <sstream>
#include <stdio.h>
#include <connected.hpp>
#include <skeleton_cuda.hpp>

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

    inline void setProcessedImage(FIELD<float> *pimg) 
    { 
      processedImage  = pimg;
      nPix = processedImage->dimX() * processedImage->dimY();
    }

    inline FIELD<float> *curImage() { return processedImage; }

    //API
    void removeIslands(float islandThreshold);
    void LayerSelection(bool cumulative, int num_layers);
    void computeSkeletons(float saliency_threshold);
    void Init_indexingSkeletons();
    void indexingSkeletons(FIELD<float> * CC, int intensity, int index);
    
    //
    void Encoding(); //To be added..
    void find_layers(int clear_color, double* importance_upper, double width);
    void calculateImportance(bool cumulative, int num_layers);
    void removeLayers();
    void CalculateCPnum(int i, FIELD<float> *imDupeCurr, int WriteToFile);

    int clear_color;

  private:
    const char *filename;
    FIELD<float>* processedImage;
    int nPix;
    double *importance;
    stringstream ofBuffer;
};

