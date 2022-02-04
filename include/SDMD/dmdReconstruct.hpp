#pragma once

#include <vector>
#include <skeleton_cuda_recon.hpp>
#include <BSplineCurveFitterWindow3.h>

#include <QMap>
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
    
    dmdReconstruct(int width_, int height_, int clear_color);
    //~dmdReconstruct();

    void openglSetup();
    void framebufferSetup();
    void renderLayer(int intensity_index);
    void renderLayer(int intensity, int nodeID);
    bool renderLayer(int intensity, vector<Vector3<float>> SampleForEachCC, bool OpenGLRenderMethod);//for action = 1 situation.
    
    FIELD<float>* renderLayer_interp(int i);


    //void readControlPoints(int width_, int height_, int clear_color, vector<int> gray_levels_);
    void readControlPoints(vector<int> gray_levels_);
    
    //void readIndexingControlPoints(int width, int height, int clear_color, multimap<int,int> Inty_Node);
    void readIndexingControlPoints(multimap<int,int> Inty_Node);
    
    QImage ReconstructImage(bool interpolate);
    void ReconstructIndexingImage(int nodeID);
    void ReconstructIndexingImage_multi(QVector<unsigned int> nodesID);
    QImage ReconstructMultiNode(bool interpolate, vector<int> nodesID, int action);


    void get_interp_layer(int i, bool last_layer);
    bool get_interp_layer(int intensity, vector<int> nodesID, bool action, bool last_layer);
    FIELD<float>* get_dt_of_alpha(FIELD<float>* alpha);
 
    FIELD<float>* renderLayer_interp(int intensity, vector<int> nodesID, bool action);
    
    void initOutput(int clear_color);


    inline FIELD<float>* getOutput() { return output; }
    inline QImage getOutQImage() { return OutImg; }
    
    void GetCPs(QVector<unsigned int> nodesID);
    void reconFromMovedCPs(QMap<unsigned int, vector<vector<Vector3<float>>>> CPlistMap);
    QImage get_texture_data(); 
    void DrawTheFirstLayer(float ClearColor);
    void RenderOutput(int inty, bool DrawAnything);
    void renderLayerInit();
    inline QMap<unsigned int, vector<vector<Vector3<float>>>> getCplistMap() 
    { return SelectedNodesCPlistMap;}
 
    
  private:

    int width, height, clearColor;
    GLfloat width_2, height_2;
    vector<int> gray_levels;
    
    FIELD<float>* output;

    QSurfaceFormat surfaceFormat;
    QOpenGLContext openGLContext;
    QOffscreenSurface surface;
    GLuint buffer,buffer2, tex, tex2, depthbuffer;
    QOpenGLFunctions *contextFunc;
    QOpenGLShaderProgram program;
    QOpenGLShaderProgram program2;
    vector<vector<Vector3<float>>> IndexingSample;
    vector<vector<vector<Vector3<float>>>> IndexingCP;
    vector<vector<Vector3<float>>> IndexingSample_interactive;
    multimap<int,int> Inty_node;
    QOpenGLBuffer vertexPositionBuffer;
    QImage OutImg;
    QMap<unsigned int, vector<vector<Vector3<float>>>> SelectedNodesCPlistMap;
};

