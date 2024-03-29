#pragma once

#include <sstream>
#include <stdio.h>
#include <connected.hpp>
#include <skeleton_cuda.hpp>

#include <BSplineCurveFitterWindow3.h>
using namespace std;
class dmdProcess {
  public:
    
    dmdProcess();
    ~dmdProcess();
    void readFromFile (const char *c_str){
      //filename = c_str;
      OriginalImage = FIELD<float>::read(c_str); 
      nPix = OriginalImage->dimX() * OriginalImage->dimY();
    }

    inline void setProcessedImage(FIELD<float> *pimg) 
    { 
      OriginalImage  = pimg;
      nPix = OriginalImage->dimX() * OriginalImage->dimY();
    }

    inline int getImgWidth(){return OriginalImage->dimX();}
    inline int getImgHeight(){return OriginalImage->dimY();}

    //inline FIELD<float> *curImage() { return processedImage; }

    //
    void removeIslands(float islandThreshold, FIELD<float> *sm);
    void LayerSelection(bool cumulative, int num_layers);
    int computeSkeletons(float saliency_threshold, float hausdorff, bool non_complement_set, FIELD<float> *sm);
    void Init_indexingSkeletons(float SalVal, float HDval, bool maxTree);
    int indexingSkeletons(FIELD<float> * CC, int intensity, int index);
    //vector<int> getIntensityOfNode(){return IntensityOfNode;}
    multimap<int,int> getInty_Node(){return Inty_node;}
    vector<int> get_gray_levels() {return gray_levels;}//without the clear_color layer
    inline vector<int> get_selected_intensity() {return selectedIntensity;}
    
    //
    void find_layers(int clear_color, double* importance_upper, double width);
    void calculateImportance(bool cumulative, int num_layers);
    void removeLayers();
    int CalculateCPnum(int i, FIELD<float> *imDupeCurr, int WriteToFile, float hausdorff, FIELD<float> *sm);
    
    int clear_color;

  private:
    //const char *filename;
    FIELD<float>* processedImage, *OriginalImage;
    int nPix;
    double *importance;
    BSplineCurveFitterWindow3 spline;
    vector<int> gray_levels;
    multimap<int,int> Inty_node;
    vector<int> selectedIntensity;
    float SalValForTree, HDvalForTree;
    bool NonComplementSet = true;
    //stringstream ofBuffer;
};

