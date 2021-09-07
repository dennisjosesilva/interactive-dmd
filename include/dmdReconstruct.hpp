#include <vector>
#include <skeleton_cuda_recon.hpp>
#include <BSplineCurveFitterWindow3.h>

//typedef Triple<int, int, int> coord3D_t;
typedef vector<Vector3<int>> layer_t;
typedef layer_t path_t;
typedef vector<layer_t*> image_t;

typedef vector<Vector4<int>> layer_index;
typedef layer_index path_index;
typedef vector<layer_index*> image_index;


class dmdReconstruct {
  public:
    
    dmdReconstruct();
    ~dmdReconstruct();

    void readControlPoints();
    void readIndexingControlPoints();
    void loadSample();
    void loadIndexingSample();
    void ReconstructImage(bool interpolate);
    void ReconstructIndexingImage(bool interpolate, int nodeID, int action);


    void get_interp_layer(int intensity, int SuperResolution, bool last_layer);
    void get_interp_index_layer(int intensity, int nodeID, int SuperResolution, bool last_layer);
    FIELD<float>* get_dt_of_alpha(FIELD<float>* alpha);
 
    layer_t *readLayer(int l);
    layer_index *readIndexLayer(int l);
    void drawEachLayer(int intensity);
    void drawEachIndexLayer(int intensity, int nodeID, int action);
 
    FIELD<float>* drawEachLayer_interp(int intensity);
    FIELD<float>* drawEachIndexLayer_interp(int intensity, int nodeID);
 
    void init();

    int width, height, clearColor;

  private:
    vector<int> gray_levels;
    image_t* r_image = nullptr;
    image_index* indexing_image = nullptr;

    FIELD<float>* output;
};

