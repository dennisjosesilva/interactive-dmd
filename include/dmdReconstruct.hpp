#include <vector>
#include <skeleton_cuda_recon.hpp>
#include <BSplineCurveFitterWindow3.h>

//typedef Triple<int, int, int> coord3D_t;
typedef vector<Vector3<int>> layer_t;
typedef layer_t path_t;
typedef vector<layer_t*> image_t;


class dmdReconstruct {
  public:
    
    dmdReconstruct();
    ~dmdReconstruct();

    void readControlPoints();
    void loadSample();
    void ReconstructImage(bool interpolate);


    void get_interp_layer(int intensity, int SuperResolution, bool last_layer);
    FIELD<float>* get_dt_of_alpha(FIELD<float>* alpha);
 
    layer_t *readLayer(int l);
    void drawEachLayer(int intensity);
    FIELD<float>* drawEachLayer_interp(int intensity);
    void init();

    int width, height, clearColor;

  private:
    vector<int> gray_levels;
    image_t* r_image = nullptr;

    FIELD<float>* output;
};

