#include <sstream>
#include "dmdReconstruct.hpp"

FIELD<float>* prev_layer;
FIELD<float>* prev_layer_dt;
bool firstTime = true;

dmdReconstruct::dmdReconstruct() {
    printf("dmdReconstruct....\n");
    RunOnce = 1;
}
/*
dmdReconstruct::~dmdReconstruct() {
    deallocateCudaMem_recon();
}*/


void dmdReconstruct::openglSetup(){

    openGLContext.setFormat(surfaceFormat);
    openGLContext.create();
    if(!openGLContext.isValid()) qDebug("Unable to create context");

    surface.setFormat(surfaceFormat);
    surface.create();
    if(!surface.isValid()) qDebug("Unable to create the Offscreen surface");

    openGLContext.makeCurrent(&surface);
    contextFunc =  openGLContext.functions();
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
                                   //"float trans_x = gl_FragCoord.x/width_2-1.0;\n"
                                   //"float trans_y = gl_FragCoord.y/height_2-1.0;\n"
                                   "float trans_x = gl_FragCoord.x;\n"
                                   "float trans_y = gl_FragCoord.y;\n"
                                   "    float alpha = ((trans_x-x0) * (trans_x-x0) + (trans_y-y0) * (trans_y-y0)) <= r*r ? 1.0 : 0.0;\n"
                                   "    gl_FragColor = vec4(alpha,alpha,alpha,alpha);\n"
                                   "    gl_FragDepth = 1.0-alpha;\n"
                                   "}\n"
                                   );
  
}


void dmdReconstruct::renderLayer(int intensity){
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
   
    layer_t *layer = readLayer(intensity);
     
    for (unsigned int k = 0; k < layer->size(); ++k) {
        x = (*layer)[k][0];
        y = height - (*layer)[k][1] - 1;
        r = (*layer)[k][2];  
 
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

//    ========SAVE IMAGE===========
    
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


FIELD<float>* dmdReconstruct::renderLayer_interp(int intensity){
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
   
    layer_t *layer = readLayer(intensity);
     
    for (unsigned int k = 0; k < layer->size(); ++k) {
        x = (*layer)[k][0];
        y = height - (*layer)[k][1] - 1;
        r = (*layer)[k][2];  
 
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

//    ========SAVE IMAGE===========
    
    float *data = (float *) malloc(width * height * sizeof (float));
    
    contextFunc->glEnable(GL_TEXTURE_2D);
    contextFunc->glBindTexture(GL_TEXTURE_2D, tex);

    // Altering range [0..1] -> [0 .. 255] 
    contextFunc->glReadPixels(0, 0, width, height, GL_RED, GL_FLOAT, data);

    FIELD<float>* CrtLayer = new FIELD<float>(width, height);
    
    for (unsigned int x = 0; x < width; ++x) 
        for (unsigned int y = 0; y < height; ++y)
        {
            unsigned int y_ = height - 1 -y;

            CrtLayer->set(x, y_, 0); //ensure that the init value is 0 everywhere.

            if(*(data + y * width + x))
                CrtLayer->set(x, y_, 1);
        
        }

    
    free(data);
    program.release();
    vertexPositionBuffer.release();
    contextFunc->glBindFramebuffer(GL_FRAMEBUFFER, 0); 

    return CrtLayer;

}

void dmdReconstruct::loadSample() {
    int SuperResolution = 1;
    image_t* img = new image_t();
    
    path_t* layer;

    int intensity, x;
    Vector3<int> CurrentPx;
    

    for (int i = 0; i < 0xff; ++i) {
        path_t *p = new path_t();
        img->push_back(p);
    }
    
    ifstream ifs("sample.txt"); 
    string str;
    ifs >> str;
    width = (int)atof(str.c_str());
    ifs >> str;
    height = (int)atof(str.c_str()); 
    ifs >> str;
    clearColor = (int)atof(str.c_str()); 
    
    ifs >> str;
    if((int)atof(str.c_str())==65536) printf("ERROR!!!!!!!!");

    while(true)
    {
        intensity = (int)atof(str.c_str());
        gray_levels.push_back(intensity);
        //of_uncompressed << "Intensity " << +intensity << endl;
        //cout<<"intensity: "<<intensity<<endl;
        
        layer = (*img)[intensity];
          
        //cout<<"layer.size() "<<layer->size()<<endl;
        while(true)
        {
            ifs >> str;
            x = (round)(atof(str.c_str()));

            if (x != 65535)
            {
                CurrentPx[0] = x*SuperResolution;
                ifs >> str;
                CurrentPx[1] = (round)(atof(str.c_str())*SuperResolution);
                ifs >> str;
                CurrentPx[2] = (round)(atof(str.c_str())*SuperResolution);
                //of_uncompressed << x << " - " << y << " - " << dt << endl;
                layer->push_back(CurrentPx);

            }
            else break;
        }
        ifs >> str;
        if((int)atof(str.c_str())==65536) break;
    }
      
    
    //of_uncompressed.close();
    ifs.close();
    r_image = img;
    
}

void dmdReconstruct::readControlPoints(){
    BSplineCurveFitterWindow3 readCPs;
    readCPs.SplineGenerate(1);//super-resolution = 1
    loadSample();
    if (RunOnce) {openglSetup(); RunOnce = false;}
    initialize_skeletonization_recon(width, height);//initCUDA
    
}

void dmdReconstruct::loadIndexingSample() {
    int SuperResolution = 1;
    image_index* img = new image_index();
    
    path_index* layer;

    int intensity, x, index;
    Vector4<int> CurrentPx;
    

    for (int i = 0; i < 0xff; ++i) {
        path_index *p = new path_index();
        img->push_back(p);
    }
    
    ifstream ifs("sample.txt"); 
    string str;
    ifs >> str;
    width = (int)atof(str.c_str());
    ifs >> str;
    height = (int)atof(str.c_str()); 
    ifs >> str;
    clearColor = (int)atof(str.c_str()); 
    
    ifs >> str;
    if((int)atof(str.c_str())==65536) printf("ERROR!!!!!!!!");

    while(true)
    {
        intensity = (int)atof(str.c_str());
        if(!count(gray_levels.begin(),gray_levels.end(), intensity)) // has not contain intensity yet
            gray_levels.push_back(intensity);
        //of_uncompressed << "Intensity " << +intensity << endl;
        //cout<<"intensity: "<<intensity<<endl;
        
        layer = (*img)[intensity];
        ifs >> str;
        index = (int)atof(str.c_str()); 
    
        //cout<<"layer.size() "<<layer->size()<<endl;
        while(true)
        {
            ifs >> str;
            x = (round)(atof(str.c_str()));

            if (x != 65535)
            {
                CurrentPx[0] = x*SuperResolution;
                ifs >> str;
                CurrentPx[1] = (round)(atof(str.c_str())*SuperResolution);
                ifs >> str;
                CurrentPx[2] = (round)(atof(str.c_str())*SuperResolution);
                CurrentPx[3] = index;
                //of_uncompressed << x << " - " << y << " - " << dt << endl;
                layer->push_back(CurrentPx);
            }
            else break;
        }
        ifs >> str;
        if((int)atof(str.c_str())==65536) break;
    }
      
    
    //of_uncompressed.close();
    ifs.close();
    indexing_image = img;
    
}

void dmdReconstruct::readIndexingControlPoints(){
    BSplineCurveFitterWindow3 readCPs;
    readCPs.ReadIndexingSpline();
    loadIndexingSample();
    if (RunOnce) {openglSetup(); RunOnce = false;}
    initialize_skeletonization_recon(width, height);//initCUDA
    
}

void dmdReconstruct::initOutput() {
    output = new FIELD<float>(width, height);
    prev_layer = new FIELD<float>(width, height);
    prev_layer_dt = new FIELD<float>(width, height);
    //cout<<"clear_color00 "<<clear_color<<endl;
    
    for (unsigned int x = 0; x < width; x++) 
        for (unsigned int y = 0; y < height; y++)
            output->set(x, y, clearColor);

}

layer_t *dmdReconstruct::readLayer(int l) {
    return (*r_image)[l];
}
layer_index *dmdReconstruct::readIndexLayer(int l) {
    return (*indexing_image)[l];
}


FIELD<float>* dmdReconstruct::get_dt_of_alpha(FIELD<float>* alpha) {
    auto alpha_dupe = alpha->dupe();
    
    auto dt = computeCUDADT(alpha_dupe);
    return dt;
}


void dmdReconstruct::get_interp_layer(int intensity, int SuperResolution, bool last_layer)
{
    bool interp_firstLayer = 0;
    int prev_intensity, prev_bound_value, curr_bound_value;
    unsigned int x, y;
    FIELD<float>* first_layer_forDT = new FIELD<float>(width, height);
    FIELD<float>* curr_layer = renderLayer_interp(intensity);
    FIELD<float>* curr_dt = get_dt_of_alpha(curr_layer);
    //debug
    stringstream skel;
    skel<<"s"<<intensity<<".pgm";
    curr_layer->writePGM(skel.str().c_str());
    stringstream dt;
    dt<<"dt"<<intensity<<".pgm";
    curr_dt->writePGM(dt.str().c_str());
   /* */
 if(interp_firstLayer) {
    if(firstTime){//first layer
        firstTime = false;
        prev_intensity = clearColor;
        prev_bound_value = clearColor;
        for (int i = 0; i < width; ++i) 
            for (int j = 0; j < height; ++j){
                if(i==0 || j==0 || i==width-1 || j==height-1)    first_layer_forDT -> set(i, j, 0); 
                else first_layer_forDT -> set(i, j, 1); 
                prev_layer -> set(i, j, 1);
            }

        prev_layer_dt = get_dt_of_alpha(first_layer_forDT);
    }
    
    curr_bound_value = (prev_intensity + intensity)/2;

    CUDA_interp(curr_layer, prev_layer, prev_layer_dt, curr_dt, curr_bound_value, prev_bound_value, false, 0);

     }
     else{
          if(firstTime){ 
              firstTime = false; 
              CUDA_interp(curr_layer, prev_layer, prev_layer_dt, curr_dt, curr_bound_value, clearColor, true, 0);//draw clear_color
             
              curr_bound_value = (clearColor + intensity)/2;
          }
          else{
            curr_bound_value = (prev_intensity + intensity)/2;
            CUDA_interp(curr_layer, prev_layer, prev_layer_dt, curr_dt, curr_bound_value, prev_bound_value, false, 0);
            
        }   
     }

    prev_bound_value = curr_bound_value;

    if(last_layer){
        output = CUDA_interp(curr_layer, prev_layer, prev_layer_dt, curr_dt, curr_bound_value, prev_bound_value, false, intensity);

    }

    prev_intensity = intensity;
    prev_layer = curr_layer->dupe();
    prev_layer_dt = curr_dt->dupe();

    delete curr_layer;
    delete curr_dt;
}

void dmdReconstruct::ReconstructImage(bool interpolate){
    initOutput();
    if(!gray_levels.empty())
    {
        for (int inty : gray_levels) {
            if(interpolate){
                bool last_layer = false;
                int SuperResolution = 1;
                int max_level = *std::max_element(gray_levels.begin(), gray_levels.begin() + gray_levels.size());

                if(inty == max_level) last_layer = true;
                get_interp_layer(inty, SuperResolution, last_layer);

            }
            else{
                renderLayer(inty);
            } 
        }

        output->NewwritePGM("output.pgm");
        printf("DMD finished!\n");

    }
    else printf("gray_levels is empty!");

}


FIELD<float>* dmdReconstruct::renderLayer_interp(int intensity, int nodeID){
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
    layer_index *layer = readIndexLayer(intensity);
    float x, y, r;
    int index;
   
    for (unsigned int k = 0; k < layer->size(); ++k) {
        x = (*layer)[k][0];
        y = height - (*layer)[k][1] - 1;
        r = (*layer)[k][2]; 
        index = (*layer)[k][3]; 

        if(index!=nodeID){
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
    }

//    ========SAVE IMAGE===========
    
    float *data = (float *) malloc(width * height * sizeof (float));
    
    contextFunc->glEnable(GL_TEXTURE_2D);
    contextFunc->glBindTexture(GL_TEXTURE_2D, tex);

    // Altering range [0..1] -> [0 .. 255] 
    contextFunc->glReadPixels(0, 0, width, height, GL_RED, GL_FLOAT, data);

    FIELD<float>* CrtLayer = new FIELD<float>(width, height);
    
    for (unsigned int x = 0; x < width; ++x) 
        for (unsigned int y = 0; y < height; ++y)
        {
            unsigned int y_ = height - 1 -y;

            CrtLayer->set(x, y_, 0); //ensure that the init value is 0 everywhere.

            if(*(data + y * width + x))
                CrtLayer->set(x, y_, 1);
        
        }

    
    free(data);
    program.release();
    vertexPositionBuffer.release();
    contextFunc->glBindFramebuffer(GL_FRAMEBUFFER, 0); 

    return CrtLayer;

}


void dmdReconstruct::get_interp_layer(int intensity, int nodeID, int SuperResolution, bool last_layer)
{
    bool interp_firstLayer = 0;
    int prev_intensity, prev_bound_value, curr_bound_value;
    unsigned int x, y;
    FIELD<float>* first_layer_forDT = new FIELD<float>(width, height);
    FIELD<float>* curr_layer = renderLayer_interp(intensity, nodeID);
    
    FIELD<float>* curr_dt = get_dt_of_alpha(curr_layer);
    /**/stringstream layer;
    layer<<"layer"<<intensity<<".pgm";
    curr_layer->NewwritePGM(layer.str().c_str());
    
 if(interp_firstLayer) {
    if(firstTime){//first layer
        firstTime = false;
        prev_intensity = clearColor;
        prev_bound_value = clearColor;
        for (int i = 0; i < width; ++i) 
            for (int j = 0; j < height; ++j){
                if(i==0 || j==0 || i==width-1 || j==height-1)    first_layer_forDT -> set(i, j, 0); 
                else first_layer_forDT -> set(i, j, 1); 
                prev_layer -> set(i, j, 1);
            }

        prev_layer_dt = get_dt_of_alpha(first_layer_forDT);
    }
    
    curr_bound_value = (prev_intensity + intensity)/2;
    CUDA_interp(curr_layer, prev_layer, prev_layer_dt, curr_dt, curr_bound_value, prev_bound_value, false, 0);

     }
     else{
          if(firstTime){ 
              firstTime = false;
              CUDA_interp(curr_layer, prev_layer, prev_layer_dt, curr_dt, curr_bound_value, clearColor, true, 0);//draw clear_color
              
              curr_bound_value = (clearColor + intensity)/2;
          }
          else{
            curr_bound_value = (prev_intensity + intensity)/2;
            CUDA_interp(curr_layer, prev_layer, prev_layer_dt, curr_dt, curr_bound_value, prev_bound_value, false, 0);
            
        }   
     }

    prev_bound_value = curr_bound_value;

    if(last_layer){
        output = CUDA_interp(curr_layer, prev_layer, prev_layer_dt, curr_dt, curr_bound_value, prev_bound_value, false, intensity);

    }

    prev_intensity = intensity;
    prev_layer = curr_layer->dupe();
    prev_layer_dt = curr_dt->dupe();

    delete curr_layer;
    delete curr_dt;
}

//can be improved by adding nodeLevel parameter
void dmdReconstruct::renderLayer(int intensity, int nodeID, int action){
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
    int index;
    bool draw = false;
   
    layer_index *layer = readIndexLayer(intensity);
     
    for (unsigned int k = 0; k < layer->size(); ++k) {
        
        index = (*layer)[k][3]; 

        if(action==0)//delete
            draw = (nodeID == index) ? false : true;
        else//hignlight
            draw = (nodeID == index) ? true : false;
        
        if(draw){
            x = (*layer)[k][0];
            y = height - (*layer)[k][1] - 1;
            r = (*layer)[k][2]; 

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

void dmdReconstruct::ReconstructIndexingImage(bool interpolate, int nodeID, int action){
    initOutput();
    if(!gray_levels.empty())
    {
        if(interpolate){
            if(action)//highlight
            {
                for (int inty : gray_levels) 
                    renderLayer(inty, nodeID, action);
            }
            else{
                bool last_layer = false;
                int SuperResolution = 1;
                int max_level = *std::max_element(gray_levels.begin(), gray_levels.begin() + gray_levels.size());
    
                vector<int>::reverse_iterator it;
                for(it = gray_levels.rbegin();it!=gray_levels.rend();it++){//draw order is very important
                    if(*it == max_level) last_layer = true;
                    get_interp_layer(*it, nodeID, SuperResolution, last_layer);
            
                }
            }
        }
        else{
            vector<int>::reverse_iterator it;
            for(it = gray_levels.rbegin();it!=gray_levels.rend();it++){//draw order is very important
                //cout<<"inty: "<<*it<<endl;
                renderLayer(*it, nodeID, action);
            }
        }

        output->NewwritePGM("output.pgm");
        printf("DMD finished!\n");

    }
    else printf("gray_levels is empty!");

}