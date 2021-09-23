#pragma once

#include <vector>
#include <skeleton_cuda_recon.hpp>
#include <BSplineCurveFitterWindow3.h>

#include <QDebug>
#include <QOffscreenSurface>
#include <QOpenGLFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QDebug>
#include <QImage>
#include <QLoggingCategory>
#include <QOpenGLTexture>

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
    //~dmdReconstruct();

    void openglSetup();
    void renderLayer(int intensity);
    void renderLayer(int intensity, int nodeID, int action);
 
    FIELD<float>* renderLayer_interp(int intensity);


    void readControlPoints();
    void readIndexingControlPoints();
    void loadSample();
    void loadIndexingSample();
    void ReconstructImage(bool interpolate);
    void ReconstructIndexingImage(bool interpolate, int nodeID, int action);


    void get_interp_layer(int intensity, int SuperResolution, bool last_layer);
    void get_interp_layer(int intensity, int nodeID, int SuperResolution, bool last_layer);
    FIELD<float>* get_dt_of_alpha(FIELD<float>* alpha);
 
    layer_t *readLayer(int l);
    layer_index *readIndexLayer(int l);
 
    FIELD<float>* renderLayer_interp(int intensity, int nodeID);
    
    void init();


    inline FIELD<float>* getOutput() { return output; }
    
  private:

    int width, height, clearColor;
    vector<int> gray_levels;
    image_t* r_image = nullptr;
    image_index* indexing_image = nullptr;

    FIELD<float>* output;

    QSurfaceFormat surfaceFormat;
    QOpenGLContext openGLContext;
    QOffscreenSurface surface;
    GLuint buffer, tex, depthbuffer;
    QOpenGLFunctions *contextFunc;
    QOpenGLShaderProgram program;
    bool RunOnce;
};

