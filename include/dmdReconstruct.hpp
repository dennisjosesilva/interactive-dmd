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


class dmdReconstruct {
  public:
    
    dmdReconstruct();
    //~dmdReconstruct();

    void openglSetup();
    void initShader();
    void framebufferSetup();
    void renderLayer(int intensity_index);
    void renderMovedLayer(int intensity, vector<Vector3<float>> SampleForEachCC);
    void renderLayer(int intensity, int nodeID);//for action = 1 situation.
    
    FIELD<float>* renderLayer_interp(int i);


    void readControlPoints(int width_, int height_, int clear_color, vector<int> gray_levels_);
    void readIndexingControlPoints(int width, int height, int clear_color, multimap<int,int> Inty_Node);
    
    QImage ReconstructImage(bool interpolate);
    void ReconstructIndexingImage(int nodeID);
    void ReconstructMultiNode(bool interpolate, vector<int> nodesID, int action);


    void get_interp_layer(int i, int SuperResolution, bool last_layer);
    void get_interp_layer(int intensity, vector<int> nodesID, bool action, bool last_layer);
    FIELD<float>* get_dt_of_alpha(FIELD<float>* alpha);
 
    FIELD<float>* renderLayer_interp(int intensity, vector<int> nodesID, bool action);
    
    void initOutput(int clear_color);


    inline FIELD<float>* getOutput() { return output; }
    vector<vector<Vector3<float>>> GetCPs(int nodeID);
    void reconFromMovedCPs(int inty, vector<vector<Vector3<float>>> CPlist);
    QImage get_texture_data(); 
    void DrawTheFirstLayer();
 
    
  private:

    int width, height, clearColor;
    vector<int> gray_levels;
    
    FIELD<float>* output;

    QSurfaceFormat surfaceFormat;
    QOpenGLContext openGLContext;
    QOffscreenSurface surface;
    GLuint buffer,buffer2, tex,tex2, depthbuffer;
    QOpenGLFunctions *contextFunc;
    QOpenGLShaderProgram program;
    QOpenGLShaderProgram program2;
    bool RunOnce = true;
    vector<vector<Vector3<float>>> IndexingSample;
    vector<vector<vector<Vector3<float>>>> IndexingCP;
    vector<vector<Vector3<float>>> IndexingSample_interactive;
    multimap<int,int> Inty_node;
    //QOpenGLTexture alpha;
};

