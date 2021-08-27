
#include "dmdProcess.hpp"

int EPSILON=0.00001;
int max_elem = 0;
int peaks;

dmdProcess::dmdProcess() {
    printf("Into dmdProcess class.");
    this->importance = NULL;
}

dmdProcess::~dmdProcess() {
    free(importance);

}

void dmdProcess::removeIslands(float islandThreshold) {
    int i, j, k;                    /* Iterators */
    FIELD<float> *inDuplicate = 0;  /* Duplicate, because of inplace modifications */
    FIELD<float> *newImg = new FIELD<float>(processedImage->dimX(), processedImage->dimY());
    int highestLabel;               /* for the CCA */
    int *ccaOut;                    /* labeled output */
    ConnectedComponents *CC;        /* CCA-object */
    float *fdata;
    unsigned int *hist;
    
    printf("Removing small islands...\n");
    /* Some basic initialization */
    memset(newImg->data(), 0, nPix * sizeof(float));

    /* Connected Component Analysis */
    #pragma omp parallel for private(i, j, k, ccaOut, CC, fdata, highestLabel, hist, inDuplicate)

    for (i = 0; i < 0xff; ++i) {
        // The below value refers to the expected number of components in an image.
        CC = new ConnectedComponents(255);
        ccaOut = new int[nPix];

        inDuplicate = (*processedImage).dupe();
        inDuplicate->threshold(i);//threshold-set..
       
        
        fdata = inDuplicate->data();
 
        /* CCA -- store highest label in 'max' -- Calculate histogram */
        highestLabel = CC->connected(fdata, ccaOut, processedImage->dimX(), processedImage->dimY(), std::equal_to<float>(), true);//true is 8-connect.
        hist = static_cast<unsigned int*>(calloc(highestLabel + 1, sizeof(unsigned int)));
        if (!hist) {
            printf("Error: Could not allocate histogram for connected components\n");
            exit(-1);
        }
        for (j = 0; j < nPix; j++) { hist[ccaOut[j]]++; }


        /* Remove small islands */
        for (j = 0; j < nPix; j++) {
           fdata[j] = (hist[ccaOut[j]] >= (islandThreshold/100*processedImage->dimX()*processedImage->dimY())) ? fdata[j] : 255 - fdata[j]; //change the absolute num. to %
             //fdata[j] = (hist[ccaOut[j]] >= islandThreshold) ? fdata[j] : 255 - fdata[j]; //change the absolute num. to %
        }
        
        #pragma omp critical
        {
            for (j = 0; j < processedImage->dimY(); j++)
                for (k = 0; k < processedImage->dimX(); k++)
                    if (0 == fdata[j * processedImage->dimX() + k] && newImg->value(k, j) < i) { newImg->set(k, j, i); }
        }

        /* Cleanup */
        free(hist);
        delete [] ccaOut;
        delete CC;
        delete inDuplicate;
    }

    for (j = 0; j < processedImage->dimY(); j++)
        for (k = 0; k < processedImage->dimX(); k++)
            processedImage->set(k, j, newImg->value(k, j));

    delete newImg;
    printf("Island removal Done!\n");
    //processedImage->writePGM("island.pgm");
  
}


void detect_peak(
    const double*   data, /* the data */
    int             data_count, /* row count of data */
    vector<int>&    emi_peaks, /* emission peaks will be put here */
    double          delta, /* delta used for distinguishing peaks */
    int             emi_first /* should we search emission peak first of
                                     absorption peak first? */
) {
    int     i;
    double  mx;
    double  mn;
    int     mx_pos = 0;
    int     mn_pos = 0;
    int     is_detecting_emi = emi_first;


    mx = data[0];
    mn = data[0];

    for (i = 1; i < data_count; ++i) {
        if (data[i] > mx) {
            mx_pos = i;
            mx = data[i];
        }
        if (data[i] < mn) {
            mn_pos = i;
            mn = data[i];
        }

        if (is_detecting_emi &&
                data[i] < mx - delta) {

            emi_peaks.push_back(mx_pos);
            is_detecting_emi = 0;

            i = mx_pos - 1;

            mn = data[mx_pos];
            mn_pos = mx_pos;
        } else if ((!is_detecting_emi) &&
                   data[i] > mn + delta) {

            is_detecting_emi = 1;

            i = mn_pos - 1;

            mx = data[mn_pos];
            mx_pos = mn_pos;
        }
    }
}

void find_peaks(double* importance, double width) {
    double impfac = 0.1;
    vector<int> v;
    int numiters = 0;
    while (numiters < 1000) {
        v.clear();
        detect_peak(importance, 256, v, impfac, 0);
        if (v.size() < width)
            impfac *= .9;
        else if (v.size() > width)
            impfac /= .9;
        else
            break;
        if(impfac < 0.0002) break;//Too small, then break
        numiters++;
    } 
    memset(importance, 0, 256 * sizeof(double));
    for (auto elem : v)
        importance[elem] = 1;
}

void detect_layers(int clear_color, double* upper_level, double threshold, bool needAssign)
{

    int distinguishable_interval = 7; //distinguishable_interval is set to 7
    peaks = 0;
    int i = clear_color;
    int StartPoint = distinguishable_interval; //There is no need to check i and i+1; it is not distinguished by eyes, so check between i and i+StartPoint.
    double difference;

    double copy_upper_level[256];

    for (int j = 0; j < 256; ++j){
        copy_upper_level [j] = upper_level [j];
    }

    while ((i + StartPoint) < (max_elem + 1))
    {
        difference = copy_upper_level[i + StartPoint] - copy_upper_level[i];//attention: here shouldn't be upper_level
        if (difference > threshold)//choose this layer
        {
            if (needAssign) {
                upper_level[i + StartPoint] = 1;
            }
            
            i = i + StartPoint;
            StartPoint = distinguishable_interval;
            peaks++; 
        }
        else
        {
            StartPoint += 1;
        }
        //cout<<"Choose_L: "<<i<<" StartPoint: "<<StartPoint<<" peaks "<<peaks<<" diff: "<<difference<<" thres: "<<threshold<<endl;   
    }

}

//binary search
void dmdProcess::find_layers(int clear_color, double* importance_upper, double width)
{
    double impfac = 0.5;
    double head = 0;
    double tail = 0.5;
    int numiters = 0;
    while (numiters < 100) {
        if(impfac < 0.003) break;//The difference is too small
        detect_layers(clear_color, importance_upper, impfac, 0);
        if (peaks < width){//impfac need a smaller one
            tail = impfac;
            impfac = (head + impfac)/2;
        }
            
        else if (peaks > width) //impfac need a bigger one
            {
                head = impfac;
                impfac = (tail + impfac)/2;
            }
            else
             break;
        numiters++;
    }
   
    detect_layers(clear_color, importance_upper, impfac, 1);//the impfac to be calculated is 0.003/2
    
     for (int i = 0; i < 256; ++i) {
         if (importance_upper[i] == 1) {
             importance[i] = 1;
         }
         else {
            importance_upper[i] = 0;
            importance[i] = 0;
        }
    }
}


/*
* Calculate the histogram of the image, which is equal to the importance for each level.
*/
void dmdProcess::calculateImportance(bool cumulative, int num_layers) {
    printf("Calculating the importance for each layer...\n");
    int normFac = 0;
    float *c = processedImage->data();
    float *end = processedImage->data() + nPix;

    int min_elem = 1e5;
    while (c != end){
        max_elem = max(max_elem, *c);
        min_elem = min(min_elem, *c++);
    }
        
    clear_color = min_elem;
    
    /* If importance was already calculated before, cleanup! */
    if (importance) { free(importance); }
    importance = static_cast<double*>(calloc(256, sizeof(double)));
    double* UpperLevelSet = static_cast<double*>(calloc(256, sizeof(double)));

    if (!importance) {
        printf("Error: could not allocate importance histogram\n");
        exit(-1);
    }

    c = processedImage->data();//back to the beginning!
    // Create a histogram
    while (c < end) {
        importance[static_cast<int>(*c++)] += 1;
    }

    UpperLevelSet[0] = importance[0];
    for (int i = 0; i < 255; i++)
        UpperLevelSet[i+1] = importance[i+1] + UpperLevelSet[i];


    // Normalize. Local-maximal method.
    normFac = static_cast<int>(*std::max_element(importance, importance + 256));//find the max one.
    for (int i = 0; i < 256; ++i) {
        importance[i] /= static_cast<double>(normFac);
    }
    // Normalize. Cumulative method.
    double max = UpperLevelSet[255];
    for (int i = 0; i < 256; ++i) {
        UpperLevelSet[i] = UpperLevelSet[i] / static_cast<double>(max) - EPSILON;//To avoid to be 1.
    }

    // Cumulative method.
    if (cumulative) {
        find_layers(clear_color, UpperLevelSet, num_layers);
    } 
    else {// else local-maxima method
        find_peaks(importance, num_layers);
    }

    importance[clear_color] = 1;
    
    std::vector<int> v;
    for (int i = 0; i < 256; ++i) {
        if (importance[i] == 1) {
            v.push_back(i);
        }
    }
    if (v.size() == 0) {
        printf("ERROR: No layers selected. Exiting...\n");
        exit(-1);
    }
    printf("Selected %lu layers: ", v.size());
    /*
    std::ostringstream ss;

    std::copy(v.begin(), v.end() - 1, std::ostream_iterator<int>(ss, ", "));
    ss << v.back();
    printf("%s\n", ss.str().c_str());
    */
    v.clear();
    max_elem = 0;
    free(UpperLevelSet);
}

void dmdProcess::removeLayers() {
    float val_up, val_dn;

    printf("Filtering image layers...\n");
    printf("The following grayscale intensities are selected: \n");
    
    for (int i = 0; i < 256; ++i) {
        if (importance[i] == 1) {
            printf("%d, ", i);
        }
    }

    for (int y = 0; y < processedImage->dimY(); y++) {
        for (int x = 0; x < processedImage->dimX(); x++) {
            val_up = processedImage->value(x, y);
            val_dn = processedImage->value(x, y);
            if (importance[(int)processedImage->value(x, y)] == 1)
                continue;
            while (val_dn >= 0 || val_up <= 256) {
                if (val_dn >= 0) {
                    if (importance[(int)val_dn] == 1) {
                        processedImage->set(x, y, val_dn);
                        break;
                    }
                }
                if (val_up <= 256) {
                    if (importance[(int)val_up] == 1) {
                        processedImage->set(x, y, val_up);
                        break;
                    }
                }
                val_dn--;
                val_up++;
                
            }
        }
    }
    processedImage->writePGM("afterlayer.pgm");

}

void dmdProcess::computeSkeletons(){
    int fboSize = initialize_skeletonization(processedImage);
    
}