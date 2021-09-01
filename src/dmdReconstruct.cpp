#include <sstream>
#include "dmdReconstruct.hpp"

FIELD<float>* prev_layer;
FIELD<float>* prev_layer_dt;
bool firstTime = true;

dmdReconstruct::dmdReconstruct() {
    printf("dmdReconstruct....\n");
}

dmdReconstruct::~dmdReconstruct() {
    deallocateCudaMem_recon();
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
}

void dmdReconstruct::init() {
    output = new FIELD<float>(width, height);
    prev_layer = new FIELD<float>(width, height);
    prev_layer_dt = new FIELD<float>(width, height);
    //cout<<"clear_color00 "<<clear_color<<endl;
    
    for (unsigned int x = 0; x < width; x++) 
        for (unsigned int y = 0; y < height; y++)
            output->set(x, y, clearColor);

    initialize_skeletonization_recon(width, height);//initCUDA
}
layer_t *dmdReconstruct::readLayer(int l) {
    return (*r_image)[l];
}

void dmdReconstruct::drawEachLayer(int intensity){
    layer_t *layer = readLayer(intensity);
    int x, y, r;
    
    for (unsigned int k = 0; k < layer->size(); ++k) {
        //x = (*layer)[k].first;
        //y = (*layer)[k].second;
        //r = (*layer)[k].third;
        x = (*layer)[k][0];
        y = (*layer)[k][1];
        r = (*layer)[k][2];

        for(int i = x-r; i < x+r+1; i++)
            for(int j = y-r; j < y+r+1; j++)
            {
                if(output->value(i,j) > clearColor) continue;
                if(i > x - (double)r/sqrt(2) && i < x + (double)r/sqrt(2) && j > y - (double)r/sqrt(2) && j > y - (double)r/sqrt(2)){
                    output->set(i, j, intensity);
                }
                else if (sqrt((i-x)*(i-x)+(j-y)*(j-y)) < r+1) output->set(i, j, intensity);
            }
        
    }

}

FIELD<float>* dmdReconstruct::drawEachLayer_interp(int intensity){
    FIELD<float>* binaryLayer = new FIELD<float>(width, height);
    
    for (unsigned int x = 0; x < width; x++) 
        for (unsigned int y = 0; y < height; y++)
            binaryLayer->set(x, y, 0);

    layer_t *layer = readLayer(intensity);
    int x, y, r;
    
    for (unsigned int k = 0; k < layer->size(); ++k) {
        
        x = (*layer)[k][0];
        y = (*layer)[k][1];
        r = (*layer)[k][2];

        for(int i = x-r; i < x+r+1; i++)
            for(int j = y-r; j < y+r+1; j++)
            {
                if(binaryLayer->value(i,j) > 0) continue;
                if(i > x - (double)r/sqrt(2) && i < x + (double)r/sqrt(2) && j > y - (double)r/sqrt(2) && j > y - (double)r/sqrt(2)){
                    binaryLayer->set(i, j, 1);
                }
                else if (sqrt((i-x)*(i-x)+(j-y)*(j-y)) < r+1) binaryLayer->set(i, j, 1);
            }
        
    }
    //debug
    /*
    std::stringstream ske;
    ske<<"skel/s"<<intensity<<".pgm";
    binaryLayer->writePGM(ske.str().c_str());
    */
    return binaryLayer;
}

FIELD<float>* dmdReconstruct::get_dt_of_alpha(FIELD<float>* alpha) {
    auto alpha_dupe = alpha->dupe();
    auto data = alpha_dupe->data();
    // Widen the image range from [0..1] to [0..255]

    for (int i = 0; i < width * height; i++) {
        data[i] *= 255;
    }
    
    auto dt = computeCUDADT(alpha_dupe);
    return dt;
}


void dmdReconstruct::get_interp_layer(int intensity, int SuperResolution, bool last_layer)
{
    bool interp_firstLayer = 1;
    int prev_intensity, prev_bound_value, curr_bound_value;
    unsigned int x, y;
    FIELD<float>* first_layer_forDT = new FIELD<float>(width, height);
    FIELD<float>* curr_layer = drawEachLayer_interp(intensity);
    FIELD<float>* curr_dt = get_dt_of_alpha(curr_layer);
    /*stringstream dt;
    dt<<"skel/dt"<<inty<<".pgm";
    curr_dt->writePGM(dt.str().c_str());
    */
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

    for (x = 0; x < width; ++x) 
        for (y = 0; y < height; ++y)
        {
            if(!curr_layer->value(x, y) && prev_layer->value(x, y))
            {// If there are pixels active between boundaries we smoothly interpolate them
                    
                    float prev_dt_val = prev_layer_dt->value(x, y);
                    float curr_dt_val = curr_dt->value(x, y);

                    float interp_alpha = prev_dt_val / ( prev_dt_val + curr_dt_val);
                    float interp_color = curr_bound_value * interp_alpha + prev_bound_value *  (1 - interp_alpha);
                    
                    output->set(x, y, interp_color);
                //}
            }
        }

     }
     else{
          if(firstTime){ 
              firstTime = false;  
              curr_bound_value = (clearColor + intensity)/2;
          }
          else{
            curr_bound_value = (prev_intensity + intensity)/2;
            
            for (x = 0; x < width; ++x) 
                for (y = 0; y < height; ++y)
                {
                    if(!curr_layer->value(x, y) && prev_layer->value(x, y))// If there are pixels active between boundaries we smoothly interpolate them
                    { 
                        float prev_dt_val = prev_layer_dt->value(x, y);
                        float curr_dt_val = curr_dt->value(x, y);

                        float interp_alpha = prev_dt_val / ( prev_dt_val + curr_dt_val);
                        float interp_color = curr_bound_value * interp_alpha + prev_bound_value *  (1 - interp_alpha);
                        
                        output->set(x, y, interp_color);
                    }
                }
        }   
     }

    prev_bound_value = curr_bound_value;

    if(last_layer){
        for (x = 0; x < width; ++x) 
            for (y = 0; y < height; ++y){
                if(curr_layer->value(x, y))
                {
                    int interp_last_layer_value = prev_bound_value + (curr_dt->value(x, y)/10);
                    int MaxIntensity = (intensity+10) > 255 ? 255 : (intensity+10);
                    output->set(x, y, (interp_last_layer_value > MaxIntensity) ? MaxIntensity : interp_last_layer_value);
                
                }
            }
    }

    prev_intensity = intensity;
    prev_layer = curr_layer->dupe();
    prev_layer_dt = curr_dt->dupe();

    delete curr_layer;
    delete curr_dt;
}

void dmdReconstruct::ReconstructImage(bool interpolate){
    init();
    if(!gray_levels.empty())
    {
        if(interpolate){
            bool last_layer = false;
            int SuperResolution = 1;
            int max_level = *std::max_element(gray_levels.begin(), gray_levels.begin() + gray_levels.size());
   
            for (int inty : gray_levels) {

                if(inty == max_level) last_layer = true;
                get_interp_layer(inty, SuperResolution, last_layer);
        
            }

        }
        else{
            vector<int>::reverse_iterator it;
            for(it = gray_levels.rbegin();it!=gray_levels.rend();it++){
                std::cout << *it << "\t";
                drawEachLayer(*it);
            }
        }

        output->NewwritePGM("output.pgm");

    }
    else printf("gray_levels is empty!");

}