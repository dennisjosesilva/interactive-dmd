#include "dmdReconstruct.hpp"


dmdReconstruct::dmdReconstruct() {
    printf("dmdReconstruct....\n");
}

dmdReconstruct::~dmdReconstruct() {
    
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

void dmdReconstruct::initOUT() {
    output = new FIELD<float>(width, height);
    //cout<<"clear_color00 "<<clear_color<<endl;
    //if(clear_color > 124 && clear_color< 130) clear_color = 128;
    int clear_color = 0;
    for (unsigned int x = 0; x < width; x++) 
        for (unsigned int y = 0; y < height; y++)
            output->set(x, y, clear_color);
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
                if(output->value(i,j) > 0) continue;
                if(i > x - (double)r/sqrt(2) && i < x + (double)r/sqrt(2) && j > y - (double)r/sqrt(2) && j > y - (double)r/sqrt(2)){
                    output->set(i, j, intensity);
                }
                else if (sqrt((i-x)*(i-x)+(j-y)*(j-y)) < r+1) output->set(i, j, intensity);
            }
        
    }

}

void dmdReconstruct::ReconstructImage(){
    initOUT();
    if(!gray_levels.empty())
    {
        vector<int>::reverse_iterator it;
        for(it = gray_levels.rbegin();it!=gray_levels.rend();it++){
            std::cout << *it << "\t";
            drawEachLayer(*it);
        }
        output->writePGM("output.pgm");

    }
    else printf("gray_levels is empty!");

}