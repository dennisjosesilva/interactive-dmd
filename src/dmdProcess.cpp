#include <math.h>
#include <vector>
#include <iostream>
//#include <cstdlib>
#include <fstream>
#include <bitset>
#include "dmdProcess.hpp"
#include <BSplineCurveFitterWindow3.h>

typedef std::pair<int, int> coord2D_t;
typedef vector<coord2D_t> coord2D_list_t;
extern float SKELETON_SALIENCY_THRESHOLD;

int EPSILON=0.00001;
int max_elem = 0;
int peaks;
FIELD<float>* skelImp;//for importance map
vector<vector<Vector3<float>>> BranchSet;
vector<int *> connection;
extern float diagonal;

dmdProcess::dmdProcess() {
    printf("Into dmdProcess class.\n");
    this->importance = NULL;
}

dmdProcess::~dmdProcess() {
    free(importance);
    //deallocateCudaMem();
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
        cout<<" "<<impfac;
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
                upper_level[i + StartPoint] = 2; //Give it a num that it can never be.
            }
            
            i = i + StartPoint;
            StartPoint = distinguishable_interval;
            peaks++; 
        }
        else
        {
            StartPoint += 1;
        }
        //std::cout<<"Choose_L: "<<i<<" StartPoint: "<<StartPoint<<" peaks "<<peaks<<" diff: "<<difference<<" thres: "<<threshold<<endl;   
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
        if(impfac < 0.03) break;//The difference is too small
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
         if (importance_upper[i] == 2) {
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
    printf("Selecting %d layers...\n", num_layers);
    int normFac = 0;
    float *c = processedImage->data();
    float *end = processedImage->data() + nPix;
   
    int min_elem = 1e5;
    while (c != end){
        //max_elem = Fieldmax(max_elem, *c);
        //min_elem = Fieldmin(min_elem, *c++);
        max_elem = (max_elem<*c)? *c : max_elem;
        min_elem = (min_elem<*c)? min_elem : *c;
        c++;
    }
    cout<< "max_elem: " << max_elem <<" min_elem: "<<min_elem<<endl;
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
cout<<" Create a histogram"<<endl;
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
    //processedImage->writePGM("afterlayer.pgm");

}

void dmdProcess::LayerSelection(bool cumulative, int num_layers){
    calculateImportance(cumulative, num_layers);
    removeLayers();
}


void analyze_cca(int level, FIELD<float>* skel) {
    int nPix = skel->dimX() * skel->dimY();
    float* ssd = skel->data();
    
    ConnectedComponents *CC = new ConnectedComponents(255);
    int                 *ccaOut = new int[nPix];
    int                 hLabel;
    unsigned int        *hist;
    int                 i;

    /* Perform connected component analysis */
    hLabel = CC->connected(ssd, ccaOut, skel->dimX(), skel->dimY(), std::equal_to<float>(), true);
    hist = static_cast<unsigned int*>(calloc(hLabel + 1, sizeof(unsigned int)));    
    if (!hist) {
        printf("Error: Could not allocate histogram for skeleton connected components\n");
        exit(-1);
    }
   
    for (i = 0; i < nPix; i++) { hist[ccaOut[i]]++; }
    float SKELETON_ISLAND_THRESHOLD = 5.0;
    /* Remove small islands */
    for (i = 0; i < nPix; i++) {
        ssd[i] = (hist[ccaOut[i]] >= SKELETON_ISLAND_THRESHOLD/100*sqrt(nPix)) ? ssd[i] : 0;
       
    }

    free(hist);
    delete CC;
    delete [] ccaOut;

    skel->FieldSetData(ssd);
   
}

/* rmObject -Remove current object in a 3x3 kernel, used for removeDoubleSkel: */
void rmObject(int *k, int x, int y) {
    if (x < 0 || x > 2 || y < 0 || y > 2 || k[y * 3 + x] == 0) { return; }
    k[y * 3 + x] = 0;
    rmObject(k, x + 1, y + 1);
    rmObject(k, x + 1, y);
    rmObject(k, x + 1, y - 1);
    rmObject(k, x, y + 1);
    rmObject(k, x, y - 1);
    rmObject(k, x - 1, y + 1);
    rmObject(k, x - 1, y);
    rmObject(k, x - 1, y - 1);
}
/* numObjects - Count the number of objects in a 3x3 kernel, used for removeDoubleSkel: */
int numObjects(int *k) {
    int c = 0;
    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; ++y) {
            if (k[y * 3 + x]) { rmObject(k, x, y); c++; }
        }
    }
    return c;
}

/**
* removeDoubleSkel
* @param FIELD<float> * layer -- the layer of which the skeleton should be reduced
* @return new FIELD<float> *. Copy of 'layer', where all redundant skeleton-points are removed (i.e. rows of width 2.)
*/
void removeDoubleSkel(FIELD<float> *layer) {
    int *k = (int *)calloc(9, sizeof(int));
    for (int y = 0; y < layer->dimY(); ++y) {
        for (int x = 0; x < layer->dimX(); ++x) {
            if (layer->value(x, y)) {
                k[0] = layer->value(x - 1, y - 1);
                k[1] = layer->value(x - 1, y);
                k[2] = layer->value(x - 1, y + 1);
                k[3] = layer->value(x  , y - 1);
                k[4] = 0;
                k[5] = layer->value(x  , y + 1);
                k[6] = layer->value(x + 1, y - 1);
                k[7] = layer->value(x + 1, y);
                k[8] = layer->value(x + 1, y + 1);
                if (k[0] + k[1] + k[2] + k[3] + k[4] + k[5] + k[6] + k[7] + k[8] > 256) {
                    int b = numObjects(k);
                    if (b < 2 ) //{layer->set(x, y, 0); } //original method. Has a problem.
                    { //wang. change a bit.
                        if(layer->value(x + 1, y) && layer->value(x + 1, y + 1)) layer->set(x+1, y, 0); //k[7] && k[8]
                        else layer->set(x, y, 0);
                    }
                }
            }
        }
    }
    free(k);
}


coord2D_list_t * neighbours(int x, int y, FIELD<float> *skel) {
    coord2D_list_t *neigh = new coord2D_list_t();
    int n[8] = {1, 1, 1, 1, 1, 1, 1, 1};

    // Check if we are hitting a boundary on the image 
    if (x <= 0 )             {        n[0] = 0;        n[3] = 0;        n[5] = 0;    }
    if (x >= skel->dimX() - 1) {        n[2] = 0;        n[4] = 0;        n[7] = 0;    }
    if (y <= 0)              {        n[0] = 0;        n[1] = 0;        n[2] = 0;    }
    if (y >= skel->dimY() - 1) {        n[5] = 0;        n[6] = 0;        n[7] = 0;    }

    // For all valid coordinates in the 3x3 region: check for neighbours
    if ((n[0] != 0) && (skel->value(x - 1, y - 1) > 0)) { neigh->push_back(coord2D_t(x - 1, y - 1)); }
    if ((n[1] != 0) && (skel->value(x    , y - 1) > 0)) { neigh->push_back(coord2D_t(x    , y - 1)); }
    if ((n[2] != 0) && (skel->value(x + 1, y - 1) > 0)) { neigh->push_back(coord2D_t(x + 1, y - 1)); }
    if ((n[3] != 0) && (skel->value(x - 1, y    ) > 0)) { neigh->push_back(coord2D_t(x - 1, y    )); }
    if ((n[4] != 0) && (skel->value(x + 1, y    ) > 0)) { neigh->push_back(coord2D_t(x + 1, y    )); }
    if ((n[5] != 0) && (skel->value(x - 1, y + 1) > 0)) { neigh->push_back(coord2D_t(x - 1, y + 1)); }
    if ((n[6] != 0) && (skel->value(x    , y + 1) > 0)) { neigh->push_back(coord2D_t(x    , y + 1)); }
    if ((n[7] != 0) && (skel->value(x + 1, y + 1) > 0)) { neigh->push_back(coord2D_t(x + 1, y + 1)); }

    return neigh;
}


void tracePath(int x, int y, FIELD<float> *skel, FIELD<float> *dt, vector<Vector3<float>> Branch, int &seq){
    int *EachConnect = (int *)calloc(4, sizeof(int));
    int index = 0;
    Vector3<float> CurrentPx;
    coord2D_t n;
    coord2D_list_t *neigh;
    
    CurrentPx[0] = (float)x/diagonal;
    CurrentPx[1] = (float)y/diagonal;
    CurrentPx[2] = (float)dt->value(x, y)/diagonal;

	//OutFile<<seq<<" " << x << " " << y << " " <<dt->value(x, y)<< " " <<skelImp->value(x,y) <<endl;//for 4D
   
    Branch.push_back(CurrentPx);
    
	skel->set(x, y, 0);
    //segment->set(x,y,255);
	neigh = neighbours(x, y, skel);

	if (neigh->size() < 1) BranchSet.push_back(Branch);//==0; at end.
	else if (neigh->size() < 2)//==1; same branch.
	{
		n = *(neigh->begin());
		tracePath(n.first, n.second, skel, dt, Branch,seq);//same seq;
	}
	else if (neigh->size() > 1) {//produce branches.
        //if (Branch.size()>1)	
        BranchSet.push_back(Branch);
        EachConnect[index] = seq;

		while (neigh->size() > 0) {
			seq++;
            index++;
			n = *(neigh->begin());
            int first = n.first;
            int second = n.second;
            neigh->erase(neigh->begin());

            for (auto i = neigh->begin(); i!=neigh->end();i++){//See the explainaton in my note
                skel->set((*i).first,(*i).second, 0);//first set to 0
               
            }
            EachConnect[index] = seq;
            vector<Vector3<float>> NewBranch;
			tracePath(first, second, skel, dt, NewBranch,seq);

            for (auto i = neigh->begin(); i!=neigh->end();i++)
                skel->set((*i).first,(*i).second, 1);//then set back to 1.
            
    	}
        connection.push_back(EachConnect);
	}
	 delete neigh; 
}

/*
 WriteToFile:
 0: Won't write to controlPoints.txt file
 1: Write - lower thresholding
 2: Write - upper thresholding
*/
void dmdProcess:: CalculateCPnum(int i, FIELD<float> *imDupeCurr, int WriteToFile)
{
    FIELD<float> *skelCurr = 0;
    int seq = 0, x, y; 
    int CPnum = 0;
    int SkelPoints = 0;
    
    skelImp = computeSkeleton(i, imDupeCurr);
    
    //NewRemoveDoubleSkel();
    //skel importance map or saliency map transforms to skel.
    skelCurr = new FIELD<float>(skelImp->dimX(), skelImp->dimY());
    for (int i = 0; i < skelImp->dimX(); ++i) {
        for (int j = 0; j < skelImp->dimY(); ++j) {
            bool is_skel_point = skelImp->value(i,j);
            skelCurr->set(i, j, is_skel_point ? 255 : 0);
        }
    } 
    
    
    analyze_cca(i, skelCurr);
    removeDoubleSkel(skelCurr);
    
    for (y = 0; y < skelCurr->dimY(); ++y) {
        for (x = 0; x < skelCurr->dimX(); ++x) {
            if (skelCurr->value(x, y) > 0) {
                if(WriteToFile>0) SkelPoints++;
                if(imDupeCurr->value(x,y) == 0)//dt=0
                    skelCurr->set(x,y,0);
            }
        }
    }
      //cout<<"enterORnot: "<<enterORnot<<" "<<i<<endl;
    if(WriteToFile>0 && SkelPoints == 0)  
        printf(" Attention: There are no skeletons produced for this layer. \n");
    
    std::stringstream ske;
    ske<<"output/s"<<i<<".pgm";
    skelCurr->writePGM(ske.str().c_str());

    ///////new method-----segment and store into the BranchSets;
     for (y = 0; y < skelCurr->dimY(); ++y) {
        for (x = 0; x < skelCurr->dimX(); ++x) {
            if (skelCurr->value(x, y) > 0) {
                vector<Vector3<float>> Branch;
                //if(imDupeCurr->value(x,y) == 0) cout<<"! ";
                tracePath(x, y, skelCurr, imDupeCurr, Branch, seq);//get the connection
                seq++;
            }
        }
    }
    //seq = 0;
            ////fit with spline///
    //cout<<"BranchSet.size()--"<<BranchSet.size()<<endl;
     //begin = std::chrono::steady_clock::now();
    float hausdorff = 0.002; //spline fitting error threshold
    if(BranchSet.size()>0){
        BSplineCurveFitterWindow3 spline;
        if (WriteToFile>0) spline.SplineFit2(BranchSet, hausdorff, diagonal, i, connection, WriteToFile);
        else CPnum = spline.SplineFit(BranchSet, hausdorff, diagonal, i, connection);
        BranchSet.clear();//important.
        connection.clear();
    }
    //end = std::chrono::steady_clock::now();
    //diff_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    //SplineTime += diff_ms;
    
 /* */
    delete skelCurr;
}

void dmdProcess::computeSkeletons(float saliency_threshold){
    SKELETON_SALIENCY_THRESHOLD = saliency_threshold;
    //ofstream clear;
    //clear.open("controlPoints.txt");
    //clear.close();

    ofstream OutFile1;
    OutFile1.open("controlPoints.txt");
    OutFile1<<processedImage->dimX()<<" "<<processedImage->dimY()<<endl; // the final end tag for one channel.
    OutFile1<<clear_color<<endl; // the final end tag for one channel.
    

    int fboSize = initialize_skeletonization(processedImage);
    //time1++;
    FIELD<float> *imDupeBack = 0;
    //FIELD<float> *imDupeFore = 0;
    //FIELD<float> *imDupe;
    //FIELD<float> *skelCurr = 0;
    //int seq = 0;
 
    diagonal  = sqrt((float)(processedImage->dimX() * processedImage->dimX() + processedImage->dimY() * processedImage->dimY()));
    bool firstTime = true;

    printf("\nComputing the skeleton for all layers...\n");
   
    for (int i = 0; i < 256; ++i) {
        if (importance[i] > 0) {
            //cout<<" i "<<i;
            if (firstTime)//Do nothing.
                firstTime = false; 
            else 
            { 
                // Threshold the image
                imDupeBack = processedImage->dupe();
                
                imDupeBack->threshold(i);

                 //debug--print layer
                /* stringstream ss;
                ss<<"output/l"<<i<<".pgm";
                imDupeBack->writePGM(ss.str().c_str());
                 */  
                bool ADAPTIVE = false;
                if(!ADAPTIVE)
                    CalculateCPnum(i,imDupeBack,2);
                else
                {
                    printf("Adaptive Layer Encoding method will be added later...\n");
                    
                }
                    
            }
        }
    }
    ofstream OutFile2;
    OutFile2.open("controlPoints.txt",ios_base::app);
    OutFile2<<65536<<endl; // the final end tag for one channel.
    
    OutFile2.close(); /**/
    printf("Skeletonization Done!\n");

    //delete skelImp;
}

void dmdProcess::Encoding(){
    //To be added...
    /*
    ofstream of;
    ofstream of_unEncode;

    of.open("EncodingOutput.sdmd", ios_base::out | ios::binary);
    of_unEncode.open("unEncode.sdmd", ios_base::out | ios::binary);

    if (!of.good() && !of_unEncode.good()) {
        printf("Could not open file for output\n");
        exit(1);
    }

    unsigned int width = processedImage->dimX();
    unsigned int height = processedImage->dimY();
    int colorspace = 1;
    int clearcolor = clear_color;
   
    ifstream ifs("controlPoints.txt"); 
    string str;
    uint8_t out8;
    int x,y,dt,imp; ////if we use uint16_t, then we cannot represent negative value.(-5 turns to 65531)
    int BranchNum, CPnum, degree;
    int last_x=0, last_y=0, last_dt=0;
    int encode_x, encode_y, encode_dt;

    of_unEncode << width << " - " << height << '\n';
    
    //writeBits(ofBuffer, bitset<16>(width).to_string() + bitset<16>(height).to_string());
    of.close();
    of_unEncode.close();

    of_uncompressed << " colorspace "  << colorspace << '\n';
    writeBits(ofBuffer, bitset<8>(colorspace).to_string());
    writeBits(ssForCR, bitset<8>(colorspace).to_string());


    out8 = clearcolor;
    ofBuffer.write(reinterpret_cast<char *> (&out8), sizeof (uint8_t));
    ssForCR.write(reinterpret_cast<char *> (&out8), sizeof (uint8_t));
   

    while(ifs)//each loop is each layer
    {
        //last_x = 0; last_y = 0; last_dt = 0;
        ifs >> str;
        if(ifs.fail())  break;

        int layerNum = (int)(atof(str.c_str()));

        if(layerNum==255){
            of_uncompressed << "11111111" << '\n';
            writeBits(ofBuffer, "11111111");
            writeBits(ssForCR, "11111111");
            
            break;
        }
       
        of_uncompressed << layerNum << '\n';
        writeBits(ofBuffer, bitset<8>(layerNum).to_string());
        writeBits(ssForCR, bitset<8>(layerNum).to_string());

        ifs >> str;
        string needToInvert = bitset<8>((int)(atof(str.c_str()))).to_string();
        of_uncompressed << (int)(atof(str.c_str())) << '\n';
        writeBits(ofBuffer, needToInvert);
        writeBits(ssForCR, needToInvert);
        
    while(true)
    {
        ifs >> str;
        CPnum = (int)(atof(str.c_str()));
        if (CPnum == 0)
        {
            of_uncompressed << CPnum << '\n';
            writeBits(ofBuffer, bitset<8>(CPnum).to_string());
            writeBits(ssForCR, bitset<8>(CPnum).to_string());
            break;
        } 
        
        string firstFourBits = bitset<4>(CPnum).to_string();
       
        ifs >> str;
        degree = (int)(atof(str.c_str()));
        string lastFourBits = bitset<4>(degree).to_string();

        //of_uncompressed << firstFourBits << " - " << lastFourBits << '\n';
        of_uncompressed << CPnum << " - " << degree << '\n';
        writeBits(ofBuffer, firstFourBits + lastFourBits);
        writeBits(ssForCR, firstFourBits + lastFourBits);

        ifs >> str;
        string SampleNum = bitset<16>((int)(atof(str.c_str()))).to_string();
        of_uncompressed << (int)(atof(str.c_str())) << '\n';
        writeBits(ofBuffer, SampleNum);

        while(CPnum--)  
        {
            ifs >> str;
            x = (atof(str.c_str()));
            ifs >> str;
            y = (atof(str.c_str()));
            ifs >> str;
            dt = (atof(str.c_str()));
            //ifs >> str;////4D
            //imp = (atof(str.c_str()));
            encode_x = x - last_x;//maybe negive value.
            encode_y = y - last_y;//maybe negive value.
            encode_dt = dt - last_dt;//maybe negive value.
            if (encode_x == 0 && encode_y == 0) {encode_x = 1;cout<<"attention the error!"<<endl;}//To avoid conflict with the flag!!
            if (encode_x > 127 || encode_y > 127 || encode_dt > 127 || encode_x < -127 || encode_y < -127 || encode_dt < -127)
            {
                of_uncompressed << "0000000000000000" << encode_x << " / " << encode_y << " / " << encode_dt << '\n';
                writeBits(ofBuffer, "0000000000000000");
                writeBits(ofBuffer, encode_16bit_point(encode_x,encode_y,encode_dt));
                writeBits(ssForCR, "0000000000000000");
                writeBits(ssForCR, encode_16bit_point(encode_x,encode_y,encode_dt));
            }
            else
            {
                of_uncompressed << encode_x << " / " << encode_y << " / " << encode_dt << '\n';
                writeBits(ofBuffer, encode_8bit_point(encode_x,encode_y,encode_dt));
                writeBits(ssForCR, encode_8bit_point(encode_x,encode_y,encode_dt));
            }
            last_x = x;
            last_y = y;
            last_dt = dt;
        } 
     }
    }  
*/
}

void dmdProcess::Init_indexingSkeletons(){
    SKELETON_SALIENCY_THRESHOLD = 0.4;//need to be improved, set by users.
    int clear_color = 0;
    int width = processedImage->dimX();
    int height = processedImage->dimY();
    diagonal  = sqrt((float)(width * width + height * height));
    
    //clear controlPoints.txt first and then write width, height, and clear_color.
    ofstream OutFile1;
    OutFile1.open("controlPoints.txt");
    OutFile1<<width<<" "<<height<<endl; // 
    OutFile1<<clear_color<<endl; // 

    int fboSize = initialize_skeletonization(processedImage);

}
//CC-connected component.
//0-foreground; 1- background.
void dmdProcess::indexingSkeletons(FIELD<float> * CC, int intensity, int index){
    bool ADAPTIVE = false;
    if(!ADAPTIVE)
    {
        FIELD<float> *skelCurr = 0;
        int seq = 0, x, y; 
        int CPnum = 0;
        int SkelPoints = 0;
        
        skelImp = computeSkeleton(intensity, CC);
        
        //skel importance map or saliency map transforms to skel.
        skelCurr = new FIELD<float>(skelImp->dimX(), skelImp->dimY());
        for (int i = 0; i < skelImp->dimX(); ++i) {
            for (int j = 0; j < skelImp->dimY(); ++j) {
                bool is_skel_point = skelImp->value(i,j);
                skelCurr->set(i, j, is_skel_point ? 255 : 0);
            }
        } 
        
        analyze_cca(intensity, skelCurr);
        removeDoubleSkel(skelCurr);
        
        for (y = 0; y < skelCurr->dimY(); ++y) {
            for (x = 0; x < skelCurr->dimX(); ++x) {
                if (skelCurr->value(x, y) > 0) {
                    SkelPoints++;
                    if(CC->value(x,y) == 0)//dt=0
                        skelCurr->set(x,y,0);
                }
            }
        }
        
        if(SkelPoints == 0)  
            printf(" Attention: There are no skeletons produced for this layer. \n");
        /*
        std::stringstream ske;
        ske<<"output/s"<<i<<".pgm";
        skelCurr->writePGM(ske.str().c_str());
        */
        ///////new method-----segment and store into the BranchSets;
        for (y = 0; y < skelCurr->dimY(); ++y) {
            for (x = 0; x < skelCurr->dimX(); ++x) {
                if (skelCurr->value(x, y) > 0) {
                    vector<Vector3<float>> Branch;
                    //if(imDupeCurr->value(x,y) == 0) cout<<"! ";
                    tracePath(x, y, skelCurr, CC, Branch, seq);//get the connection
                    seq++;
                }
            }
        }
        ////fit with spline///
        
        float hausdorff = 0.002; //spline fitting error threshold
        if(BranchSet.size()>0){
            BSplineCurveFitterWindow3 spline;
            spline.indexingSpline(BranchSet, hausdorff, diagonal, intensity, index);//
            
            BranchSet.clear();//important.
            connection.clear();
        }
       
        delete skelCurr;
    }
    else
        printf("Adaptive Layer Encoding method will be added later...\n");
    
}