#include <vector>
#include <skeleton_cuda.hpp>
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
    void ReconstructImage();
    layer_t *readLayer(int l);
    void drawEachLayer(int intensity);
    void initOUT();

    int width, height;

  private:
    vector<int> gray_levels;
    image_t* r_image = nullptr;

    FIELD<float>* output;
};

