
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
    
    // CV_RETR_EXTERNAL to not find holes
    int mode = (find_holes)?CV_RETR_LIST:CV_RETR_EXTERNAL;
    
    cvFindContours(img, stor, &contours, sizeof(CvContour), mode, CV_CHAIN_APPROX_SIMPLE);
    
    blobz.clear();
    while (contours) {
        ofxCvComplexBlob b =  ofxCvComplexBlob(getNewId(), contours);
        b.setApproxFactor(approxFactor);
        b.getApproxPoints();
        b.getHullPoints();
        blobz.push_back( b );
        contours = contours->h_next;
    }
    
    // sort blobs
    sort(blobz.begin(),  blobz.end(), sort_blob_func);
}


void ofxCvBlobFinder::updatePersistentBlobs()
{
    int blobIndex;
    bool blobExists;
    int i,j;
    
    //Remove expired blobs
    if(blobz.size() < m_activeBlobs.size() )
    {
        blobExists = false;

        for(i = 0; i < m_activeBlobs.size(); i++){
            int blobIndex = 0;
            int existingBlobIndex;
            
            for(j = 0; j < blobz.size(); j++)
            {
                existingBlobIndex = getNearestExistingBlobIndex(blobz[j]);
                
                if(existingBlobIndex >= 0){
                    blobExists = true;
                    blobIndex = j;
                    break;
                }                
            }
            
            ofLog(OF_LOG_NOTICE, "in removing: existingId:" + ofToString(existingBlobIndex) + " newId:" + ofToString(blobIndex));
            
            if(blobExists)
                copyBlobParameters(m_activeBlobs[existingBlobIndex], blobz[blobIndex]);
            else
                m_activeBlobs.erase(m_activeBlobs.begin() + i);
        }
    }
    
    //Update old blobs and add new ones
    else if( blobz.size() >= m_activeBlobs.size() )
    {
        for(i = 0; i < blobz.size(); i++)
        {
            blobIndex = getNearestExistingBlobIndex(blobz[i]);
            
            ofLog(OF_LOG_NOTICE, "in adding: existingId:" + ofToString(blobIndex) + " newId:" + ofToString(blobz[i].getId()) );

            if(blobIndex >= 0)
                copyBlobParameters(m_activeBlobs[blobIndex], blobz[i]);
            else
                m_activeBlobs.push_back(blobz[i]);
            
        }
    }
}

void ofxCvBlobFinder::copyBlobParameters(ofxCvComplexBlob dst, ofxCvComplexBlob src)
{
    double dstId = dst.getId();
    dst = ofxCvComplexBlob(dstId, src.points);
}



int ofxCvBlobFinder::getNearestExistingBlobIndex(ofxCvComplexBlob blob)
{
    int blobIndex = -1;
    float smallestDist = 0.0f;
    float distThreshold = 5.0f;
    
    for(int i = 0; i < m_activeBlobs.size(); i++)
    {
        float dist = blob.getBoundingBox().getCenter().distance( m_activeBlobs[i].getBoundingBox().getCenter() );
        
        if(smallestDist == 0.0f) smallestDist = dist;
        
        if( dist < distThreshold && dist <= smallestDist){
            blobIndex = i;
            smallestDist = dist;
        }
    }
    
    return blobIndex;
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
    for (int j = 0; j < m_activeBlobs.size(); j++) {
        ofSetHexColor(0xFF0000);
        DRAW_BLOB_VECTOR( m_activeBlobs[j].getPoints());
        
        ofSetHexColor(0x00FF00);
        DRAW_BLOB_VECTOR(m_activeBlobs[j].getHullPoints());
        
        ofSetHexColor(0x0000FF);
        DRAW_BLOB_VECTOR( m_activeBlobs[j].getApproxPoints());
        
        ofSetHexColor(0x00ffae);
        ofRectangle c = m_activeBlobs[j].getBoundingBox();
        
        
        // draw bounding box
        ostringstream s;
        s << j << "Area = " << m_activeBlobs[j].getArea();
        
        //ofDrawBitmapString(s.str(), c.x, c.y);
        ofRect(c.x, c.y, c.width, c.height);
        
        // get convexity defects
        vector<ofxCvConvexityDefect> cd = m_activeBlobs[j].getConvexityDefects();
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
    
    glPopMatrix();
}

void ofxCvBlobFinder::reset() {
    blobz.clear();
    m_activeBlobs.clear();
}

#endif
