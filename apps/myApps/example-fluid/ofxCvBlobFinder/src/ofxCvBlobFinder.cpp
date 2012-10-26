
#ifndef OFXCVBLOBFINDER_H
#define OFXCVBLOBFINDER_H

#include "ofxCvBlobFinder.h"

//--------------------------------------------------------------------------------
bool sort_blob_func( ofxCvComplexBlob a, ofxCvComplexBlob b){
    return a.getArea() > b.getArea();
}


//----------------------------------------------------------------------------------
ofxCvBlobFinder::ofxCvBlobFinder()
{
    approxFactor = 0.005;
    idCount = 0;
}


//----------------------------------------------------------------------------------
void ofxCvBlobFinder::findBlobs(ofxCvGrayscaleImage image, bool find_holes) {
    
    CvMemStorage *stor = cvCreateMemStorage();
    IplImage *img = image.getCvImage();
    CvSeq *contours;
    
    _width = img->width;
    _height = img->height;
    
    // CV_RETR_EXTERNAL to not find holes
    int mode = (find_holes)?CV_RETR_LIST:CV_RETR_EXTERNAL;
    
    cvFindContours(img, stor, &contours, sizeof(CvContour), mode, CV_CHAIN_APPROX_SIMPLE);
    
    blobz.clear();
    while (contours) {
        ofxCvComplexBlob b =  ofxCvComplexBlob(contours);
        b.setApproxFactor(approxFactor);
        b.getApproxPoints();
        b.getHullPoints();
        blobz.push_back( b );
        contours = contours->h_next;
    }
    
    // sort blobs
    sort(blobz.begin(),  blobz.end(), sort_blob_func);
}



void ofxCvBlobFinder::matchExistingBlobs(){
    int i;
    int paramIndex;
    
    if(blobz.size() < 1) blobParams.clear();
    
    //Create and delete notes based on blob detection outcome
    
    if(blobz.size() > blobParams.size() )  //Create new notes
    {
        for(i = 0; i < blobz.size(); i++)
        {
            paramIndex = getParamIndexFromBlob(blobz[i]);
           
            if(paramIndex < 0 ){
                BlobParam blobParam;
                blobParam.id = getNewId();
                blobParam.isDirty = true;
                blobParam.area = 0;
                blobParam.position = ofPoint(0,0);
                blobParam.curvature = 0;
                blobParams.push_back(blobParam);
            }
        }
    }
    
    //Flag expired blobs
    else if( blobz.size() < blobParams.size() )     
    {
        for(i = 0; i < blobParams.size(); i++){
            int blobIndex = 0;
            while( blobIndex < blobz.size() )
            {
                paramIndex = getParamIndexFromBlob(blobz[blobIndex]);
                
                if(paramIndex < 0){
                    blobParams.erase(blobParams.begin() + i);
                    break;
                }
                
                blobIndex++;
            }
        }
    }
    
    //Update blobparam properties
    if(blobz.size() == blobParams.size() )
    {
        for(i = 0; i < blobParams.size(); i++)
        {
            paramIndex = getParamIndexFromBlob(blobz[i], true);
            
            float area = blobz[i].getArea() / MAX_BLOB_AREA;
            ofPoint position(blobz[i].getBoundingBox().getCenter().x / _width, blobz[i].getBoundingBox().getCenter().y / _height);
            
            vector<ofxCvConvexityDefect> cd = blobz[i].getConvexityDefects();
            float curvature = 0.0f;
            float largestDefect;
            float smallestDefect;
            for(int j=0; j < cd.size(); j++){
                curvature += cd[j].length;
            }
            
            curvature = CLAMP(1.0f - ((MAX_CURVATURE - (curvature / cd.size() + 1)) / MAX_CURVATURE), 0.0f, 1.0f);
            
            if(area != blobParams[paramIndex].area || blobParams[paramIndex].position != position || blobParams[paramIndex].position != curvature){
                blobParams[paramIndex].isDirty = true;
                blobParams[paramIndex].position = position;
                blobParams[paramIndex].area = area;
                blobParams[paramIndex].curvature = curvature;                
            }
        }
    }
}



int ofxCvBlobFinder::getParamIndexFromBlob(ofxCvComplexBlob blob, bool force)
{
    int noteIndex = -1;
    float smallestDist = 0.0f;
    float distThreshold = 10.0f;
    
    if(blobz.size() == 1)
        return 0;
    
    for(int i = 0; i < blobParams.size(); i++)
    {
        float dist = blob.getBoundingBox().getCenter().distance( blobParams[i].position );
        
        if(smallestDist == 0.0f) smallestDist = dist;
        
        if( dist <= smallestDist){
            if(!force && dist < distThreshold){
                noteIndex = i;
                smallestDist = dist;
            } else {
                noteIndex = i;
                smallestDist = dist;
            }
        }
    }
    
    return noteIndex;
}


/*
 * Match active notes against current list of blobs
 */
int ofxCvBlobFinder::getParamIndexFromBlob(ofxCvComplexBlob blob)
{
    getParamIndexFromBlob(blob, false);
}









//----------------------------------------------------------------------------------
void ofxCvBlobFinder::draw(float x, float y, float w, float h) {
    
    // draw blobs
    //ofxCvContourFinder::draw(x, y, w, h);
    // scale blob
    float scalex = 0.0f;
    float scaley = 0.0f;
    
    if (_width != 0) {
        scalex = w / _width;
    }
    else {
        scalex = 1.0f;
    }
    
    if (_height != 0) {
        scaley = h / _height;
    }
    else {
        scaley = 1.0f;
    }
    
    
    ofSetPolyMode(OF_POLY_WINDING_NONZERO);
    // apply transformation
    glPushMatrix();
    glTranslatef(x, y, 0.0);
    glScalef(scalex, scaley, 0.0);
    
    
#define DRAW_BLOB_VECTOR(points) do{ \
ofBeginShape(); \
for(int i = 0; i < (points).size(); i++){ \
ofVertex((points[i]).x, (points[i]).y); \
} \
ofEndShape(true); \
} while(0)
    
    ofNoFill();
    for (int j = 0; j < blobz.size(); j++) {
        ofSetHexColor(0xFF0000);
        DRAW_BLOB_VECTOR( blobz[j].getPoints());
        
        ofSetHexColor(0x00FF00);
        DRAW_BLOB_VECTOR(blobz[j].getHullPoints());
        
        ofSetHexColor(0x0000FF);
        DRAW_BLOB_VECTOR( blobz[j].getApproxPoints());
        
        ofSetHexColor(0x00ffae);
        ofRectangle c = blobz[j].getBoundingBox();
        
        
        // draw bounding box
        ostringstream s;
        s << j << "Area = " << blobz[j].getArea();
        
        ofDrawBitmapString(s.str(), c.x, c.y + 50);
        ofRect(c.x, c.y, c.width, c.height);
        
        // get convexity defects
        vector<ofxCvConvexityDefect> cd = blobz[j].getConvexityDefects();
        ofSetHexColor(0x00effe);
        for(int i=0; i < cd.size(); i++){
            ofLine(cd[i].start.x, cd[i].start.y, cd[i].end.x, cd[i].end.y);
            ofCircle(cd[i].defect.x, cd[i].defect.y, 2);
            float angle = atan2f( ( (float) (cd[i].end.y - cd[i].start.y) ) , ( (float) (cd[i].end.x - cd[i].start.x)));
            float x = cd[i].defect.x - sinf(angle) * cd[i].length;
            float y = cd[i].defect.y + cosf(angle) * cd[i].length;
            ofSetHexColor(0xF0F0F0);
            ofLine(cd[i].defect.x, cd[i].defect.y, x, y);
        }
    }
    
    for (int k = 0; k < blobParams.size(); k++){
        ofDrawBitmapString("Blob:" + ofToString(blobParams[k].id), blobParams[k].position);
    }
    
    glPopMatrix();
}

void ofxCvBlobFinder::reset() {
    blobz.clear();
}

#endif
