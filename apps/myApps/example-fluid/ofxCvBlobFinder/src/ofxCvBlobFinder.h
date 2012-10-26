/*

 FIXME:
  - add blob sorting by area
  
*/

#ifndef __ofxCvBlobFinder__
#define __ofxCvBlobFinder__



#include "ofxOpenCv.h"
#include "ofxCvComplexBlob.h"

#define MAX_BLOB_AREA 60000
#define MAX_CURVATURE 20.0f

struct BlobParam
{
    int id;
    bool isDirty;
    int curvature;
    ofPoint position;
    float intensity;
    float area;
};

class ofxCvBlobFinder : private ofBaseDraws {

  public:
    ofxCvBlobFinder();
  
    virtual float getWidth(){ return _width; };
    virtual float getHeight(){ return _height; };
    void findBlobs(ofxCvGrayscaleImage img, bool find_hole);
    
    void setApproxFactor(float factor){ approxFactor = factor; }; 
    float getApproxFactor(){ return approxFactor; }; 
    
    void matchExistingBlobs();
    int getParamIndexFromBlob(ofxCvComplexBlob blob);
    int getParamIndexFromBlob(ofxCvComplexBlob blob, bool force);

    virtual void  draw() {
      draw(0, 0, _width, _height);
    };

    virtual void  draw(float x, float y) {
      draw(x, y, _width, _height);
    };

    virtual void  draw(float x, float y, float w, float h);   //{ ofxCvContourFinder::draw(x, y, w, h);  };

    void reset();
    int getNumBlobs(){ return blobz.size(); };
    
    int getNewId(){return idCount++; };
    void setBlobClean(int blobParamIndex){ blobParams[blobParamIndex].isDirty = false; };

    vector <BlobParam> blobParams;

  private:
    
    vector <ofxCvComplexBlob> blobz;
    
    int _width, _height, idCount;
    float approxFactor;
};

#endif

