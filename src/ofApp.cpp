#include "ofApp.h"

using namespace cv;
using namespace ofxCv;

//---------------------------------------------------------------------
void histMatch(const Mat &reference, const Mat &target, const Mat &result)
{
    const float HISTMATCH = 0.000001;  // threshold of histogram difference
    double min, max;
    
    vector<Mat> ref_channels;  // 
    split(reference, ref_channels);
    vector<Mat> tgt_channels;  // 
    split(target, tgt_channels);
    
    int histSize = 256;  // ]
    float range[] = { 0, 256 };  // 
    const float *histRange = { range };
    bool uniform = true;
    
    for (int i = 0; i < 3; i++)
    {
        Mat ref_hist, tgt_hist;  // 
        Mat ref_hist_accum, tgt_hist_accum;  // CDF
        
        calcHist(&ref_channels[i], 1, 0, Mat(), ref_hist, 1, &histSize, &histRange, uniform, false);
        calcHist(&tgt_channels[i], 1, 0, Mat(), tgt_hist, 1, &histSize, &histRange, uniform, false);
        
        minMaxLoc(ref_hist, &min, &max);  // 
        if (max == 0)
        {
            cout << "ERROR: max is 0 in ref_hist" << endl;
        }
        normalize(ref_hist, ref_hist, min / max, 1.0, NORM_MINMAX);  // normalize
        
        minMaxLoc(tgt_hist, &min, &max);  
        if (max == 0)
        {
            cout << "ERROR: max is 0 in tgt_hist" << endl;
        }
        normalize(tgt_hist, tgt_hist, min / max, 1.0, NORM_MINMAX);  // normalize
        
        //
        // CDF를 계산한다.
        //
        ref_hist.copyTo(ref_hist_accum);  // 
        tgt_hist.copyTo(tgt_hist_accum);
        
        float *src_cdf_data = ref_hist_accum.ptr<float>();  // for speed
        float *dst_cdf_data = tgt_hist_accum.ptr<float>();
        
        for (int j = 1; j < 256; j++)
        {
            src_cdf_data[j] += src_cdf_data[j - 1];
            dst_cdf_data[j] += dst_cdf_data[j - 1];
        }
        
        //
        // CDF normalize
        //
        minMaxLoc(ref_hist_accum, &min, &max);
        normalize(ref_hist_accum, ref_hist_accum, min / max, 1.0, NORM_MINMAX);
        minMaxLoc(tgt_hist_accum, &min, &max);
        normalize(tgt_hist_accum, tgt_hist_accum, min / max, 1.0, NORM_MINMAX);
        
        //
        // Histogram matching
        //
        Mat lut(1, 256, CV_8UC1);  // Lookup table
        uchar *M = lut.ptr<uchar>();
        uchar last = 0;
        for (int j = 0; j < tgt_hist_accum.rows; j++)
        {
            float F1 = dst_cdf_data[j];
            
            //
            // intensity search
            //
            for (uchar k = last; k < ref_hist_accum.rows; k++)
            {
                float F2 = src_cdf_data[k];
                if (abs(F2 - F1) < HISTMATCH || F2 > F1)  // 
                {
                    M[j] = k;  // 
                    last = k;  // 
                    break;  // 
                }
                
            }
        }
        
        LUT(tgt_channels[i], lut, tgt_channels[i]);  // color change
    }  // end of for
    
    merge(tgt_channels, result);  // merge
    
}
//--------------------------------------------------------------
void ofApp::setup(){
    
    string filepath2= "/Users/mun/Desktop/image.jpg";
    string filepath1= "/Users/mun/Desktop/yard.png";
    of1.load(filepath1);
    of2.load(filepath2);

    Mat ref= toCv(of1);
    Mat tgt= toCv(of2);
    
    if (ref.empty() == true)
    {
        cout << "Unable to read reference image" << endl;
        return -1;
    }

    if (tgt.empty() == true)
    {
        cout << "Unable to read target image" << endl;
        return -1;
    }
    Mat dst = tgt.clone();
    
    histMatch(ref, tgt, dst);
    
    toOf(ref, of1);
    toOf(tgt, of2);
    toOf(dst, of3);

}

//--------------------------------------------------------------
void ofApp::update(){


    of1.update();
    of2.update();
    of3.update();
}

//--------------------------------------------------------------
void ofApp::draw(){

    ofSetColor(255, 255, 255);
    of1.draw(0, 0, 512, 512);
    of2.draw(512, 0, 512, 512);
    of3.draw(1024, 0, 512, 512);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
