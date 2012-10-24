/*

 FIXME:
  - add blob sorting by area
  
*/



#include "ofxOpenCv.h"
#include "ofxCvComplexBlob.h"

class ofxCvBlobFinder : private ofBaseDraws {

  public:
    ofxCvBlobFinder();
    
    double getNewId(){ return idCount++; };
  
    virtual float getWidth(){ return _width; };
    virtual float getHeight(){ return _height; };
    void findBlobs(ofxCvGrayscaleImage img, bool find_hole);
    
    void setApproxFactor(float factor){ approxFactor = factor; }; 
    float getApproxFactor(){ return approxFactor; };
    
    void updatePersistentBlobs();
    void copyBlobParameters(ofxCvComplexBlob dst, ofxCvComplexBlob src);
    int getNearestExistingBlobIndex(ofxCvComplexBlob blob);

    virtual void  draw() {
      draw(0, 0, _width, _height);
    };

    virtual void  draw(float x, float y) {
      draw(x, y, _width, _height);
    };

    virtual void  draw(float x, float y, float w, float h);   //{ ofxCvContourFinder::draw(x, y, w, h);  };

    void reset();
        
    vector <ofxCvComplexBlob> blobz;
    
    int getNumBlobs(){ return blobz.size(); };

  private:
    int _width, _height;
    float approxFactor;
    double idCount;
    
    vector<ofxCvComplexBlob> m_activeBlobs; 
};

