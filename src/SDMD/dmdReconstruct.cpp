#include <sstream>
#include "SDMD/dmdReconstruct.hpp"
#define SET_TEXTURE(arr, i, val) do { (arr)[(4 * (i))] = (arr)[(4 * (i) + 1)] = (arr)[(4 * (i) + 2)] = (val); (arr)[(4 * (i) + 3)] = 255.0; } while(0);

FIELD<float>* prev_layer;
FIELD<float>* prev_layer_dt;
bool firstTime, DrawnTheLayer;
//int CurrNode;

dmdReconstruct::dmdReconstruct()
: vertexPositionBuffer(QOpenGLBuffer::VertexBuffer)
{
    printf("dmdReconstruct....\n");
    //RunOnce = 1;
}
/*
dmdReconstruct::~dmdReconstruct() {
    deallocateCudaMem_recon();
}*/

void dmdReconstruct::GetCPs(QVector<unsigned int> nodesID){
    if(!SelectedNodesCPlistMap.empty()) SelectedNodesCPlistMap.clear();
    QVector<unsigned int>::iterator it;
    for(it = nodesID.begin();it!=nodesID.end();it++){
        //SelectedNodesCPlistMap.insert(pair <unsigned int, vector<vector<Vector3<float>>> > (*it, IndexingCP.at(IndexingCP.size() - *it)));
        SelectedNodesCPlistMap.insert(*it, IndexingCP.at(IndexingCP.size() - *it) );
    }
    //cout<<"SelectedNodesCPlistMap "<<SelectedNodesCPlistMap.size();
}

void dmdReconstruct::reconFromMovedCPs(QMap<unsigned int, vector<vector<Vector3<float>>>> CPlistMap)
{
    vector<Vector3<float>> movedSample;
    BSplineCurveFitterWindow3 movedSpline;
    int inty;
    initOutput(0);
    renderLayerInit();

    for(auto it = CPlistMap.begin(); it != CPlistMap.end(); ++it)
    {
        unsigned int NodeId = it.key();
        vector<vector<Vector3<float>>> CPlist = it.value();
    
        movedSample = movedSpline.ReadIndexingSpline(CPlist);//loadSample();
        //find intendity.
        for (auto const& it_ : Inty_node) { 
            if(it_.second == (int)NodeId)
                inty = it_.first;
        }

        renderLayerInit();
        renderLayer(inty, movedSample, false);
        
        //update IndexingCP.
        IndexingCP[IndexingCP.size() - NodeId] = CPlist;///
        //update IndexingSample
        IndexingSample[IndexingSample.size() - NodeId] = movedSample;
    }
    delete output;
 
}

void dmdReconstruct::openglSetup(){

    openGLContext.setFormat(surfaceFormat);
    openGLContext.create();
    if(!openGLContext.isValid()) qDebug("Unable to create context");

    surface.setFormat(surfaceFormat);
    surface.create();
    if(!surface.isValid()) qDebug("Unable to create the Offscreen surface");

    openGLContext.makeCurrent(&surface);
                 
//    ========GEOMEETRY SETUP========

    program.addShaderFromSourceCode(QOpenGLShader::Vertex,
                                   "#version 330\r\n"
                                   "in vec2 position;\n"
                                   "void main() {\n"
                                   "    gl_Position = vec4(position, 0.0, 1.0);\n"
                                   "}\n"
                                   );
    program.addShaderFromSourceCode(QOpenGLShader::Fragment,
                                   "#version 330\r\n"
                                   "uniform float r;\n"
                                   "uniform float x0;\n"
                                   "uniform float y0;\n"
                                   "void main() {\n"
                                   "float trans_x = gl_FragCoord.x;\n"
                                   "float trans_y = gl_FragCoord.y;\n"
                                   "    float alpha = ((trans_x-x0) * (trans_x-x0) + (trans_y-y0) * (trans_y-y0)) <= r*r ? 1.0 : 0.0;\n"
                                   "    gl_FragColor = vec4(alpha,alpha,alpha,alpha);\n"
                                   "    gl_FragDepth = 1.0-alpha;\n"
                                   "}\n"
                                   );
    program.link();

    program2.addShaderFromSourceCode(QOpenGLShader::Vertex,
                                   "#version 330\r\n"
                                   "in vec2 Pos;\n"
                                   "in vec2 texCoord;\n"
                                   "out vec2 outCoord;\n"
                                   "void main() {\n"
                                   "    gl_Position = vec4(Pos, 0.0, 1.0);\n"
                                   "    outCoord = texCoord;\n"
                                   "}\n"
                                   );
    program2.addShaderFromSourceCode(QOpenGLShader::Fragment,
                                   "#version 330\r\n"
                                   "in vec2 outCoord;\n"
                                   "uniform sampler2D alpha;\n"
                                   "uniform float layer;\n"
                                   "void main() {\n"
                                   "    float alpha_val = texture2D(alpha, outCoord).s;\n"
                                   "    gl_FragColor=vec4(layer, layer, layer, alpha_val);\n"
                                   "}\n"
                                   );
   
    program2.link(); 
}

void dmdReconstruct::framebufferSetup(){

    contextFunc =  openGLContext.functions();
    //the first buffer.

    //generate Framebuffer;
    contextFunc->glGenFramebuffers(1, &buffer);
    contextFunc->glBindFramebuffer(GL_FRAMEBUFFER, buffer);

    //generate tex and bind to Framebuffer;
    contextFunc->glGenTextures(1, &tex);
    contextFunc->glBindTexture(GL_TEXTURE_2D, tex);
    contextFunc->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                    GL_RGBA, GL_UNSIGNED_BYTE, NULL); 
    contextFunc->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    contextFunc->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
    contextFunc->glBindTexture(GL_TEXTURE_2D, 0);
        
    contextFunc->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                    GL_TEXTURE_2D, tex, 0);
/// Attach depth buffer 
    contextFunc->glGenRenderbuffers(1, &depthbuffer);
    contextFunc->glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
    contextFunc->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    contextFunc->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);
       
    contextFunc->glViewport(0,0, width, height);

    contextFunc->glBindFramebuffer(GL_FRAMEBUFFER, 0);                                
  
  //the second buffer.
    
    //generate Framebuffer;
    contextFunc->glGenFramebuffers(1, &buffer2);
    contextFunc->glBindFramebuffer(GL_FRAMEBUFFER, buffer2);

    //generate tex and bind to Framebuffer;
    contextFunc->glGenTextures(1, &tex2);
    contextFunc->glBindTexture(GL_TEXTURE_2D, tex2);
    contextFunc->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                    GL_RGBA, GL_UNSIGNED_BYTE, NULL); 
    contextFunc->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    contextFunc->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
    contextFunc->glBindTexture(GL_TEXTURE_2D, 0);
        
    contextFunc->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                    GL_TEXTURE_2D, tex2, 0);

    contextFunc->glViewport(0,0, width, height);

    contextFunc->glBindFramebuffer(GL_FRAMEBUFFER, 0);                                
  
}

void dmdReconstruct::renderLayer(int intensity_index){
    
    renderLayerInit();
    
    //read each skeleton point
    float x, y, r;
    vector<Vector3<float>> SampleForEachInty;
    Vector3<float> EachSample;
    
    SampleForEachInty = IndexingSample_interactive.at(intensity_index);
    
    for(auto it = SampleForEachInty.begin();it!=SampleForEachInty.end();it++){
        EachSample = *it;
       
        x = EachSample[0];
        y = height - EachSample[1] - 1;
        r = EachSample[2]; 

        float vertexPositions[] = {
        (x-r)/width_2 - 1,   (y-r)/height_2 - 1,
        (x-r)/width_2 - 1,   (y+r+1)/height_2 - 1,
        (x+r+1)/width_2 - 1, (y+r+1)/height_2 - 1,
        (x+r+1)/width_2 - 1, (y-r)/height_2 - 1,
        };

        vertexPositionBuffer.allocate(vertexPositions, 8 * sizeof(float));
       
        program.enableAttributeArray("position");
        program.setAttributeBuffer("position", GL_FLOAT, 0, 2, 2*sizeof (float));
        
        program.setUniformValue("r", (GLfloat)r);
        
        program.setUniformValue("x0", (GLfloat)x);
        
        program.setUniformValue("y0", (GLfloat)y);

        contextFunc->glDrawArrays(GL_QUADS, 0, 4);

    }
}

void dmdReconstruct::RenderOutput(int inty, bool DrawAnything)
{
    program.release();
    vertexPositionBuffer.release();
    contextFunc->glBindFramebuffer(GL_FRAMEBUFFER, 0); 

    if(DrawAnything)
    {
    // SECOND PASS: Draw in the default framebuffer - Render using alpha map
        contextFunc->glBindFramebuffer(GL_FRAMEBUFFER, buffer2); 
        contextFunc->glDisable(GL_DEPTH_TEST);
        contextFunc->glEnable(GL_BLEND);
        contextFunc->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
        //program2.link();
        program2.bind();

        QOpenGLBuffer vertexPositionBuffer2(QOpenGLBuffer::VertexBuffer);
        vertexPositionBuffer2.create();
        //vertexPositionBuffer2.setUsagePattern(QOpenGLBuffer::StaticDraw);
        vertexPositionBuffer2.bind();
        float vertexPositions2[] = {
            -1.0f, -1.0f,  0.0f, 1.0f,
            -1.0f,  1.0f,  0.0f, 0.0f,
            1.0f,  1.0f,  1.0f, 0.0f,
            1.0f, -1.0f,  1.0f, 1.0f
        };

        vertexPositionBuffer2.allocate(vertexPositions2, 16 * sizeof(float));
        
        program2.enableAttributeArray("Pos");
        program2.setAttributeBuffer("Pos", GL_FLOAT, 0, 2, 4*sizeof (float));
        program2.enableAttributeArray("texCoord");
        program2.setAttributeBuffer("texCoord", GL_FLOAT, 2*sizeof (float), 2, 4*sizeof (float));
        
        
        contextFunc->glActiveTexture(GL_TEXTURE0);
        contextFunc->glBindTexture(GL_TEXTURE_2D, tex);
        program2.setUniformValue("alpha", 0);

        //int inty = gray_levels.at(intensity_index);
        //cout<<"inty: "<<inty<<endl;
        program2.setUniformValue("layer", (GLfloat)(inty/255.0));
        //program2.setUniformValue("layer", (GLfloat)(inty));

        contextFunc->glDrawArrays(GL_QUADS, 0, 4);

        //contextFunc->glBindTexture(GL_TEXTURE_2D, 0); 
        contextFunc->glDisable(GL_BLEND);

        program2.release();
        vertexPositionBuffer2.release();
        
        contextFunc->glBindFramebuffer(GL_FRAMEBUFFER, 0); 
    
    }
    
}

FIELD<float>* dmdReconstruct::renderLayer_interp(int i){
    
    renderLayerInit();
    //read each skeleton point
    float x, y, r;
    vector<Vector3<float>> SampleForEachInty;
    Vector3<float> EachSample;
    
    SampleForEachInty = IndexingSample_interactive.at(i);
    for(auto it = SampleForEachInty.begin();it!=SampleForEachInty.end();it++){
        EachSample = *it;
        x = EachSample[0];
        y = height - EachSample[1] - 1;
        r = EachSample[2]; 

        float vertexPositions[] = {
        (x-r)/width_2 - 1,   (y-r)/height_2 - 1,
        (x-r)/width_2 - 1,   (y+r+1)/height_2 - 1,
        (x+r+1)/width_2 - 1, (y+r+1)/height_2 - 1,
        (x+r+1)/width_2 - 1, (y-r)/height_2 - 1,
        };

            
        vertexPositionBuffer.allocate(vertexPositions, 8 * sizeof(float));
        
        program.enableAttributeArray("position");
        program.setAttributeBuffer("position", GL_FLOAT, 0, 2);
        
        program.setUniformValue("r", (GLfloat)r);
        
        program.setUniformValue("x0", (GLfloat)x);
        
        program.setUniformValue("y0", (GLfloat)y);

        contextFunc->glDrawArrays(GL_QUADS, 0, 4);

    }
       
    //========SAVE IMAGE===========

    float *data = (float *) malloc(width * height * sizeof (float));
    
    contextFunc->glEnable(GL_TEXTURE_2D);
    contextFunc->glBindTexture(GL_TEXTURE_2D, tex);

    // Altering range [0..1] -> [0 .. 255] 
    contextFunc->glReadPixels(0, 0, width, height, GL_RED, GL_FLOAT, data);

    FIELD<float>* CrtLayer = new FIELD<float>(width, height);
    //change to setAll();
    for (unsigned int x = 0; x < width; ++x) 
        for (unsigned int y = 0; y < height; ++y)
        {
            //unsigned int y_ = height - 1 -y;

            if(*(data + y * width + x))
                CrtLayer->set(x, y, 1);
            else CrtLayer->set(x, y, 0); 
        
        }
    
    free(data);
    program.release();
    vertexPositionBuffer.release();
    contextFunc->glBindFramebuffer(GL_FRAMEBUFFER, 0); 

    return CrtLayer;
}

void dmdReconstruct::readControlPoints(int width_, int height_, int clear_color, vector<int> gray_levels_){
    //cout<<"gray_levels_ size: "<<gray_levels_.size()<<endl;
    BSplineCurveFitterWindow3 readCPs;
    IndexingSample_interactive = readCPs.SplineGenerate();//loadSample();
    
    width = width_;
    height = height_;
    
    width_2 = (GLfloat)width/2.0;
    height_2 = (GLfloat)height/2.0;

    clearColor = clear_color;
    gray_levels = gray_levels_;
    if (RunOnce) {openglSetup(); RunOnce = false;}
    
    framebufferSetup(); 
    initialize_skeletonization_recon(width, height);//initCUDA
    
}

void dmdReconstruct::readIndexingControlPoints(int width_, int height_, int clear_color, multimap<int,int> Inty_Node){
   /**/ BSplineCurveFitterWindow3 readCPs;
    IndexingSample = readCPs.ReadIndexingSpline();//get the reconstructed skeleton points
    IndexingCP = readCPs.get_indexingCP();
    width = width_;
    height = height_;
    width_2 = (GLfloat)width/2.0;
    height_2 = (GLfloat)height/2.0;

    clearColor = clear_color;
    Inty_node = Inty_Node;
    if (RunOnce) {openglSetup(); RunOnce = false;}
    framebufferSetup(); 
    initialize_skeletonization_recon(width, height);//initCUDA
    
}

void dmdReconstruct::initOutput(int clear_color) {
    output = new FIELD<float>(width, height);
    // prev_layer = new FIELD<float>(width, height);
    // prev_layer_dt = new FIELD<float>(width, height);
   
    for (unsigned int x = 0; x < width; x++) 
        for (unsigned int y = 0; y < height; y++)
            output->set(x, y, clear_color);

}

FIELD<float>* dmdReconstruct::get_dt_of_alpha(FIELD<float>* alpha) {
    auto alpha_dupe = alpha->dupe();
    
    auto dt = computeCUDADT(alpha_dupe);
    return dt;
}

QImage dmdReconstruct::get_texture_data() { 
    float *data = (float *) malloc(width * height * sizeof (float));
        
    contextFunc->glBindFramebuffer(GL_FRAMEBUFFER, buffer2);

    contextFunc->glReadPixels(0, 0, width, height, GL_RED, GL_FLOAT, data);

    QImage img{width, height, QImage::Format_Grayscale8};
  
    uchar *img_data = img.bits();

    int N = width * height;
    for (int i = 0; i < N; ++i)
        img_data[i] = static_cast<uchar>(data[i]*255);
    
    free(data);
    contextFunc->glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return img;
} 

void dmdReconstruct::get_interp_layer(int i, bool last_layer)
{
    unsigned char *texdata = (unsigned char *) calloc(width * height * 4, sizeof (char));
    
    bool interp_firstLayer = true;
    int prev_intensity;
    unsigned int x, y;
    
    int intensity = gray_levels.at(i);
    FIELD<float>* curr_layer = renderLayer_interp(i);
    
    //debug
    /* if(firstTime || last_layer){
    stringstream skel;
    skel<<"out/s"<<intensity<<".pgm";
    curr_layer->writePGM(skel.str().c_str());
    }
    */
    FIELD<float>* curr_dt = get_dt_of_alpha(curr_layer);
    float* curr_alpha_data = curr_layer->data();
    float* curr_dt_data = curr_dt->data();
    float* prev_alpha_data = nullptr;
    float* prev_dt_data = nullptr;

    if(firstTime){//first layer
        firstTime = false;
        if(interp_firstLayer){
            FIELD<float>* first_layer_forDT = new FIELD<float>(width, height);
            prev_intensity = clearColor;
            
            for (int i = 0; i < width; ++i) 
                for (int j = 0; j < height; ++j){
                    if(i==0 || j==0 || i==width-1 || j==height-1)    first_layer_forDT -> set(i, j, 0); 
                    else first_layer_forDT -> set(i, j, 1); 
                    prev_layer -> set(i, j, 1);
                }

            prev_layer_dt = get_dt_of_alpha(first_layer_forDT);
        }
        else{
            for (int i = 0; i < width * height; ++i) {
                SET_TEXTURE(texdata, i, 255);
            }
        }  
    }

    prev_alpha_data = prev_layer->data();
    prev_dt_data = prev_layer_dt->data();

    //CUDA_interp(curr_layer, prev_layer, prev_layer_dt, curr_dt, curr_bound_value, prev_bound_value, false, 0);
    for (int i = 0; i < width * height; ++i) {
            // If the current foreground is set we set it to that
            if (curr_alpha_data[i]) {
                SET_TEXTURE(texdata, i, 255);
            } else {
                // If there are pixels active between boundaries we smoothly interpolate them
                if (prev_alpha_data[i]) {
                    float prev_dt_val = prev_dt_data[i];
                    float curr_dt_val = curr_dt_data[i];

                    //float interp_color = 0.5 * (min(1, prev_dt_val / curr_dt_val) * prev_intensity + max(1 - curr_dt_val / prev_dt_val, 0) * intensity);
                   
                    float B = prev_dt_val / (prev_dt_val + curr_dt_val);
                    float interp_color = B * 255;
                    SET_TEXTURE(texdata, i, interp_color);
                }
            }
            // Otherwise we keep the previous set value
        }
  
    /*
    if(last_layer){
        //output = CUDA_interp(curr_layer, prev_layer, prev_layer_dt, curr_dt, curr_bound_value, prev_bound_value, false, intensity);
        
        for (x = 0; x < width; ++x) 
            for (y = 0; y < height; ++y){
                if(curr_layer->value(x, y))
                {
                    int interp_last_layer_value = prev_bound_value + (curr_dt->value(x, y)/5);
                    int MaxIntensity = (intensity+10) > 255 ? 255 : (intensity+10);
                    output->set(x, y, (interp_last_layer_value > MaxIntensity) ? MaxIntensity : interp_last_layer_value);

                }
            }
    }*/

    prev_intensity = intensity;
    prev_layer = curr_layer->dupe();
    prev_layer_dt = curr_dt->dupe();

    contextFunc->glBindFramebuffer(GL_FRAMEBUFFER, buffer);
    contextFunc->glEnable(GL_TEXTURE_2D);
    contextFunc->glBindTexture(GL_TEXTURE_2D, tex);
    contextFunc->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, texdata);
    //contextFunc->glBindFramebuffer(GL_FRAMEBUFFER, 0); 

    free(texdata);
    delete curr_layer;
    delete curr_dt;
}

void dmdReconstruct::DrawTheFirstLayer(float ClearColor)
{
    contextFunc->glBindFramebuffer(GL_FRAMEBUFFER, buffer2);
    float clear_color = ClearColor / 255.0;
    contextFunc->glClearColor(clear_color, clear_color, clear_color, 0);
    contextFunc->glClear(GL_COLOR_BUFFER_BIT);
    contextFunc->glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FIELD<float>* dmdReconstruct::renderLayer_interp(int intensity, vector<int> nodesID, bool action){
    renderLayerInit();

    //read each skeleton point
    float x, y, r;
    bool DrawTheNode;
    DrawnTheLayer=false;
    vector<Vector3<float>> SampleForEachCC;
    Vector3<float> EachSample;
    
    auto it1 = Inty_node.lower_bound(intensity);
    auto it2 = Inty_node.upper_bound(intensity);
    while(it1 != it2){//process all nodes for the current intensity
        int node_id = it1->second;
        ++it1;
        SampleForEachCC = IndexingSample.at(IndexingSample.size() - node_id);
        
        if(SampleForEachCC.empty()) {
            DrawTheNode = false;
            //cout<<"The component of Node-"<<node_id<<" is too small to generate any skeletons."<<endl;
        }
        else{
            if(action) {
                if(std::find(nodesID.begin(), nodesID.end(), node_id) == nodesID.end())//if nodesID doesn't contain node_id
                    DrawTheNode = false;
                else DrawTheNode = true;
            }
            else{
                if(std::find(nodesID.begin(), nodesID.end(), node_id) == nodesID.end()) //if nodesID doesn't contain node_id
                    DrawTheNode = true;
                else DrawTheNode = false;
            }
        }
        if(DrawTheNode){
            DrawnTheLayer = true; 
            
            //FIELD<float>* skel = new FIELD<float>(width, height);
            // for (unsigned int x = 0; x < width; ++x) 
            //     for (unsigned int y = 0; y < height; ++y)
            //         skel->set(x, y, 0);
            for(auto it = SampleForEachCC.begin();it!=SampleForEachCC.end();it++){
                EachSample = *it;
                x = EachSample[0];
                y = height - EachSample[1] - 1;
                r = EachSample[2]; 

                //skel->set(x, height-1-y, 1);

                float vertexPositions[] = {
                (x-r)/width_2 - 1,   (y-r)/height_2 - 1,
                (x-r)/width_2 - 1,   (y+r+1)/height_2 - 1,
                (x+r+1)/width_2 - 1, (y+r+1)/height_2 - 1,
                (x+r+1)/width_2 - 1, (y-r)/height_2 - 1,
                };

                    
                vertexPositionBuffer.allocate(vertexPositions, 8 * sizeof(float));
                
                program.enableAttributeArray("position");
                program.setAttributeBuffer("position", GL_FLOAT, 0, 2);
                
                program.setUniformValue("r", (GLfloat)r);
                
                program.setUniformValue("x0", (GLfloat)x);
                
                program.setUniformValue("y0", (GLfloat)y);

                contextFunc->glDrawArrays(GL_QUADS, 0, 4);

            }
            // stringstream skelname;
            // skelname<<"out/skel"<<intensity<<".pgm";
            // skel->writePGM(skelname.str().c_str());
        
        }
    }

    //========SAVE IMAGE===========
       
    float *data = (float *) malloc(width * height * sizeof (float));
    
    contextFunc->glEnable(GL_TEXTURE_2D);
    contextFunc->glBindTexture(GL_TEXTURE_2D, tex);

    // Altering range [0..1] -> [0 .. 255] 
    contextFunc->glReadPixels(0, 0, width, height, GL_RED, GL_FLOAT, data);

    FIELD<float>* CrtLayer = new FIELD<float>(width, height);
    
    for (unsigned int x = 0; x < width; ++x) 
        for (unsigned int y = 0; y < height; ++y)
        {
           // unsigned int y_ = height - 1 -y;

            if(*(data + y * width + x))
                CrtLayer->set(x, y, 1);
            else CrtLayer->set(x, y, 0); 
        }

    free(data);
    program.release();
    vertexPositionBuffer.release();
    contextFunc->glBindFramebuffer(GL_FRAMEBUFFER, 0); 

    return CrtLayer;

}

bool dmdReconstruct::get_interp_layer(int intensity, vector<int> nodesID, bool action, bool last_layer)
{
    unsigned char *texdata = (unsigned char *) calloc(width * height * 4, sizeof (char));
    
    bool interp_firstLayer = true;
    int prev_intensity;
    unsigned int x, y;
    
    FIELD<float>* curr_layer = renderLayer_interp(intensity, nodesID, action);
    if(!DrawnTheLayer)
    {
        cout<<"This layer is empty."<<endl;;
        return false;
    }
    
    //debug
    /* if(firstTime || last_layer){
    stringstream skel;
    skel<<"out/s"<<intensity<<".pgm";
    curr_layer->writePGM(skel.str().c_str());
    }
    */
    FIELD<float>* curr_dt = get_dt_of_alpha(curr_layer);
    float* curr_alpha_data = curr_layer->data();
    float* curr_dt_data = curr_dt->data();
    float* prev_alpha_data = nullptr;
    float* prev_dt_data = nullptr;

    if(firstTime){//first layer
        firstTime = false;
        if(interp_firstLayer){
            FIELD<float>* first_layer_forDT = new FIELD<float>(width, height);
            prev_intensity = clearColor;
            
            for (int i = 0; i < width; ++i) 
                for (int j = 0; j < height; ++j){
                    if(i==0 || j==0 || i==width-1 || j==height-1)    first_layer_forDT -> set(i, j, 0); 
                    else first_layer_forDT -> set(i, j, 1); 
                    prev_layer -> set(i, j, 1);
                }

            prev_layer_dt = get_dt_of_alpha(first_layer_forDT);
        }
        else{
            for (int i = 0; i < width * height; ++i) {
                SET_TEXTURE(texdata, i, 255);
            }
        }  
    }

    prev_alpha_data = prev_layer->data();
    prev_dt_data = prev_layer_dt->data();

    //CUDA_interp(curr_layer, prev_layer, prev_layer_dt, curr_dt, curr_bound_value, prev_bound_value, false, 0);
    for (int i = 0; i < width * height; ++i) {
            // If the current foreground is set we set it to that
            if (curr_alpha_data[i]) {
                SET_TEXTURE(texdata, i, 255);
            } else {
                // If there are pixels active between boundaries we smoothly interpolate them
                if (prev_alpha_data[i]) {
                    float prev_dt_val = prev_dt_data[i];
                    float curr_dt_val = curr_dt_data[i];

                    //float interp_color = 0.5 * (min(1, prev_dt_val / curr_dt_val) * prev_intensity + max(1 - curr_dt_val / prev_dt_val, 0) * intensity);
                   
                    float B = prev_dt_val / (prev_dt_val + curr_dt_val);
                    float interp_color = B * 255;
                    SET_TEXTURE(texdata, i, interp_color);
                }
            }
            // Otherwise we keep the previous set value
        }
  
    /*
    if(last_layer){
        //output = CUDA_interp(curr_layer, prev_layer, prev_layer_dt, curr_dt, curr_bound_value, prev_bound_value, false, intensity);
        
        for (x = 0; x < width; ++x) 
            for (y = 0; y < height; ++y){
                if(curr_layer->value(x, y))
                {
                    int interp_last_layer_value = prev_bound_value + (curr_dt->value(x, y)/5);
                    int MaxIntensity = (intensity+10) > 255 ? 255 : (intensity+10);
                    output->set(x, y, (interp_last_layer_value > MaxIntensity) ? MaxIntensity : interp_last_layer_value);

                }
            }
    }*/

    prev_intensity = intensity;
    prev_layer = curr_layer->dupe();
    prev_layer_dt = curr_dt->dupe();

    contextFunc->glBindFramebuffer(GL_FRAMEBUFFER, buffer);
    contextFunc->glEnable(GL_TEXTURE_2D);
    contextFunc->glBindTexture(GL_TEXTURE_2D, tex);
    contextFunc->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, texdata);
    //contextFunc->glBindFramebuffer(GL_FRAMEBUFFER, 0); 

    free(texdata);
    delete curr_layer;
    delete curr_dt;
    return true;
    
}
void dmdReconstruct::renderLayerInit()
{
    program.bind();

    contextFunc->glBindFramebuffer(GL_FRAMEBUFFER, buffer);

    contextFunc->glEnable(GL_DEPTH_TEST);
    contextFunc->glClear(GL_DEPTH_BUFFER_BIT);
    contextFunc->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    contextFunc->glClear(GL_COLOR_BUFFER_BIT);
    
    vertexPositionBuffer.create();
    vertexPositionBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vertexPositionBuffer.bind();
}

bool dmdReconstruct::renderLayer(int intensity, vector<Vector3<float>> SampleForEachCC, bool OpenGLRenderMethod){
    //cout<<"nodeID: "<<nodeID<<endl;
    //vector<Vector3<float>> SampleForEachCC;
    Vector3<float> EachSample;
    
    //SampleForEachCC = IndexingSample.at(IndexingSample.size() - nodeID);
    if(!SampleForEachCC.empty()){
        
        //read each skeleton point
        float x, y, r;
        
        for(auto it = SampleForEachCC.begin();it!=SampleForEachCC.end();it++){
            EachSample = *it;
            x = EachSample[0];
            y = height - EachSample[1] - 1;
            r = EachSample[2]; 

            float vertexPositions[] = {
            (x-r)/width_2 - 1,   (y-r)/height_2 - 1,
            (x-r)/width_2 - 1,   (y+r+1)/height_2 - 1,
            (x+r+1)/width_2 - 1, (y+r+1)/height_2 - 1,
            (x+r+1)/width_2 - 1, (y-r)/height_2 - 1,
            };

                
            vertexPositionBuffer.allocate(vertexPositions, 8 * sizeof(float));
            
            program.enableAttributeArray("position");
            program.setAttributeBuffer("position", GL_FLOAT, 0, 2);
            
            program.setUniformValue("r", (GLfloat)r);
            
            program.setUniformValue("x0", (GLfloat)x);
            
            program.setUniformValue("y0", (GLfloat)y);

            contextFunc->glDrawArrays(GL_QUADS, 0, 4);

        }
         
        if(!OpenGLRenderMethod){
            //========SAVE IMAGE in the output.pgm ===========
            float *data = (float *) malloc(width * height * sizeof (float));
            
            contextFunc->glEnable(GL_TEXTURE_2D);
            contextFunc->glBindTexture(GL_TEXTURE_2D, tex);

            // Altering range [0..1] -> [0 .. 255] 
            contextFunc->glReadPixels(0, 0, width, height, GL_RED, GL_FLOAT, data);


            for (unsigned int x = 0; x < width; ++x) 
                for (unsigned int y = 0; y < height; ++y)
                {
                    unsigned int y_ = height - 1 -y;

                    if(*(data + y * width + x))
                        output->set(x, y_, intensity);
                }
            
            free(data);
            output->NewwritePGM("output.pgm");
            
            program.release();
            vertexPositionBuffer.release();
            contextFunc->glBindFramebuffer(GL_FRAMEBUFFER, 0); 
        }

        return true;
    }
    else 
    {
        cout<<"The component of the current node is too small to generate any skeletons."<<endl;
        return false;
    }
}

void dmdReconstruct::ReconstructIndexingImage(int nodeID){
    //if(nodeID > 0) CurrNode = nodeID;
    vector<Vector3<float>> SampleForEachCC;
    
    initOutput(0);
    if(!IndexingSample.empty()){
        
        int LastInty = 256;
        bool found = false;
        for (auto const& it : Inty_node) { //for each graylevel(from small num to large num).
            if(LastInty != it.first){
                LastInty = it.first;
                //cout<<"LastInty "<<LastInty<<"\t";
                
                if(nodeID != 0){
                    auto it1 = Inty_node.lower_bound(it.first);
                    auto it2 = Inty_node.upper_bound(it.first);
                    while(it1 != it2){//process all nodes for the current intensity
                        int node_id = it1->second;
                        if(node_id == nodeID){
                            renderLayerInit();
                            SampleForEachCC = IndexingSample.at(IndexingSample.size() - nodeID);
    
                            renderLayer(it.first, SampleForEachCC, false);
                           
                            found = true;
                            break;
                        }
                        ++it1;
                    }
                    if(found) break;
                }
                
            }
        }
        
        //output->NewwritePGM("output.pgm");
        delete output;
        printf("DMD finished!\n");
    }

    else printf("gray_levels is empty!");

}


void dmdReconstruct::renderLayer(int intensity, int nodeID){
    //cout<<"nodeID: "<<nodeID<<endl;
    vector<Vector3<float>> SampleForEachCC;
    Vector3<float> EachSample;
    
    SampleForEachCC = IndexingSample.at(IndexingSample.size() - nodeID);
    if(!SampleForEachCC.empty()){
        program.link();
        program.bind();

    //    ==============DRAWING TO THE FBO============

        contextFunc->glBindFramebuffer(GL_FRAMEBUFFER, buffer);

        contextFunc->glEnable(GL_DEPTH_TEST);
        contextFunc->glClear(GL_DEPTH_BUFFER_BIT);
        contextFunc->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        contextFunc->glClear(GL_COLOR_BUFFER_BIT);


        QOpenGLBuffer vertexPositionBuffer(QOpenGLBuffer::VertexBuffer);
        vertexPositionBuffer.create();
        vertexPositionBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
        vertexPositionBuffer.bind();

        GLfloat width_2 = (GLfloat)width/2.0;
        //program.setUniformValue("width_2", width_2);
        GLfloat height_2 = (GLfloat)height/2.0;
        //program.setUniformValue("height_2", height_2);

        //read each skeleton point
        float x, y, r;
        
        for(auto it = SampleForEachCC.begin();it!=SampleForEachCC.end();it++){
            EachSample = *it;
            x = EachSample[0];
            y = height - EachSample[1] - 1;
            r = EachSample[2]; 

            float vertexPositions[] = {
            (x-r)/width_2 - 1,   (y-r)/height_2 - 1,
            (x-r)/width_2 - 1,   (y+r+1)/height_2 - 1,
            (x+r+1)/width_2 - 1, (y+r+1)/height_2 - 1,
            (x+r+1)/width_2 - 1, (y-r)/height_2 - 1,
            };

                
            vertexPositionBuffer.allocate(vertexPositions, 8 * sizeof(float));
            
            program.enableAttributeArray("position");
            program.setAttributeBuffer("position", GL_FLOAT, 0, 2);
            
            program.setUniformValue("r", (GLfloat)r);
            
            program.setUniformValue("x0", (GLfloat)x);
            
            program.setUniformValue("y0", (GLfloat)y);

            contextFunc->glDrawArrays(GL_QUADS, 0, 4);

        }
        
        //========SAVE IMAGE===========
            float *data = (float *) malloc(width * height * sizeof (float));
            
            contextFunc->glEnable(GL_TEXTURE_2D);
            contextFunc->glBindTexture(GL_TEXTURE_2D, tex);

            // Altering range [0..1] -> [0 .. 255] 
            contextFunc->glReadPixels(0, 0, width, height, GL_RED, GL_FLOAT, data);


            for (unsigned int x = 0; x < width; ++x) 
                for (unsigned int y = 0; y < height; ++y)
                {
                    unsigned int y_ = height - 1 -y;

                    if(*(data + y * width + x))
                        output->set(x, y_, intensity);
                }
            
            free(data);
    
        program.release();
        vertexPositionBuffer.release();
        contextFunc->glBindFramebuffer(GL_FRAMEBUFFER, 0); 
    }
    else cout<<"The component of Node-"<<nodeID<<" is too small to generate any skeletons."<<endl;
}

void dmdReconstruct::ReconstructIndexingImage_multi(QVector<unsigned int> nodesID){

    if(!nodesID.empty()){
        //process the background color
        if(std::find(nodesID.begin(), nodesID.end(), 0) != nodesID.end()) //if nodesID contains 0
            initOutput(clearColor);
        else initOutput(0);
        
        
        if(!IndexingSample.empty()){
            
            int LastInty = 256;
            for (auto const& it : Inty_node) { //for each graylevel(from small num to large num).
                if(LastInty != it.first){//for each graylevel, only enter once.
                    LastInty = it.first;
                   
                    auto it1 = Inty_node.lower_bound(it.first);
                    auto it2 = Inty_node.upper_bound(it.first);
                    while(it1 != it2){//process all nodes for the current intensity
                        int node_id = it1->second;
                        if(std::find(nodesID.begin(), nodesID.end(), node_id) != nodesID.end()){//if nodesID contains node_id
                            renderLayer(it.first, node_id);  
                        }
                        ++it1;
                    } 
                }
            }
            
            output->NewwritePGM("output.pgm");
            // delete output;
            printf("DMD finished!\n");
        }
    }
    else printf("Nothing is selected!");
}

QImage dmdReconstruct::ReconstructImage(bool interpolate){
    QImage outImg;
    
    if(interpolate){
        firstTime = true;
        prev_layer = new FIELD<float>(width, height);
        prev_layer_dt = new FIELD<float>(width, height);
    }

    DrawTheFirstLayer(clearColor);
    
    if(!gray_levels.empty())
    {
        for (int i = 0; i < gray_levels.size(); i++) {
            int inty = gray_levels.at(i);
            if(interpolate){
                
                bool last_layer = false;
                
                int max_level = *std::max_element(gray_levels.begin(), gray_levels.begin() + gray_levels.size());

                if(inty == max_level) last_layer = true;
                get_interp_layer(i, last_layer);
                
            }
            else{
                renderLayer(i);
            } 

            RenderOutput(inty, true);
        }
        outImg = get_texture_data();

        //output->NewwritePGM("output.pgm");
        printf("DMD finished!\n");

    }
    else printf("gray_levels is empty!");
    return outImg;
}

QImage dmdReconstruct::ReconstructMultiNode(bool interpolate, vector<int> nodesID, int action){
    //sort(nodesID.begin(),nodesID.end());//sort it from small num to large num, i,e., from root to leaves
    bool DrawAnything;
    vector<Vector3<float>> SampleForEachCC;
    
    if(!nodesID.empty()){

        if(interpolate){
            firstTime = true;//for interpolation process.
            prev_layer = new FIELD<float>(width, height);
            prev_layer_dt = new FIELD<float>(width, height);
        }
        //process the background color
        if(action)
        {
            if(std::find(nodesID.begin(), nodesID.end(), 0) != nodesID.end()) //if nodesID contains 0
                DrawTheFirstLayer(clearColor);
            else DrawTheFirstLayer(0);
        } 
        else{
            if(std::find(nodesID.begin(), nodesID.end(), 0) == nodesID.end()) //if nodesID doesn't contains 0
                DrawTheFirstLayer(clearColor); 
            else DrawTheFirstLayer(0);
        }
        
        if(!IndexingSample.empty()){
            
            int LastInty = 256;
            for (auto const& it : Inty_node) { //for each graylevel(from small num to large num).
                if(LastInty != it.first){//for each graylevel, only enter once.
                    LastInty = it.first;
                    //cout<<"it.first "<<it.first<<endl;
                    if(action){//highlight
                        if(interpolate){
                            bool last_layer = false; 
                            auto [max_level, max] = *std::max_element(Inty_node.begin(), Inty_node.end());
                            
                            if(it.first == (int)max_level) last_layer = true;
                            DrawAnything = get_interp_layer(it.first, nodesID, 1, last_layer);
                        }
                        else 
                        {
                            auto it1 = Inty_node.lower_bound(it.first);
                            auto it2 = Inty_node.upper_bound(it.first);
                            DrawAnything = false;
                            renderLayerInit();
                            while(it1 != it2){//process all nodes for the current intensity
                                int node_id = it1->second;
                                SampleForEachCC = IndexingSample.at(IndexingSample.size() - node_id);
    
                                if(std::find(nodesID.begin(), nodesID.end(), node_id) != nodesID.end()){//if nodesID contains node_id
                                    if(renderLayer(it.first, SampleForEachCC, true)) DrawAnything = true;
                                }
                                ++it1;
                            } 
                        } 
                        RenderOutput(it.first, DrawAnything);
                    }
                    else{//else recon without the selected nodes-CCs
                        if(interpolate){//Add interpolate later
                            bool last_layer = false; 
                            auto [max_level, max] = *std::max_element(Inty_node.begin(), Inty_node.end());
                            //int max_level = std::max_element(Inty_node.begin(), Inty_node.end());
                            
                            if(it.first == (int)max_level) last_layer = true;
                            DrawAnything = get_interp_layer(it.first, nodesID, 0, last_layer);
                    
                        }
                        else{
                            auto it1 = Inty_node.lower_bound(it.first);
                            auto it2 = Inty_node.upper_bound(it.first);
                            DrawAnything = false;
                            renderLayerInit();
                            while(it1 != it2){//process all nodes for the current intensity
                                int node_id = it1->second;
                                SampleForEachCC = IndexingSample.at(IndexingSample.size() - node_id);
                                if(std::find(nodesID.begin(), nodesID.end(), node_id) == nodesID.end()){//if nodesID doesn't contain node_id
                                    if(renderLayer(it.first, SampleForEachCC, true)) DrawAnything = true;
                                    //cout<< "node_id: "<<node_id<<endl;
                                }
                                ++it1;
                            } 
                        }
                        RenderOutput(it.first, DrawAnything);
                    }
                }
            }
            OutImg = get_texture_data();
           
            //output->NewwritePGM("output.pgm");
            printf("DMD finished!\n");
        }
    }
    else printf("Nothing is selected!");
    
    return OutImg;
}