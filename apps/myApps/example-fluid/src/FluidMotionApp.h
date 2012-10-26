#pragma once

#include "ofMain.h"
#include "ofxFX.h"
#include "ofxCvBlobFinder.h"
#include "ofxMidi.h"
#include "ofxOpenNI.h"

#include "FluidPlayer.h"
#include "FluidKinect.h"
#include "ChannelBlender.h"

using namespace std;

class FluidMotionApp : public ofBaseApp{
public:
    
    
    //Openframeworks default functions
    void setup();
    void update();
    void draw();
    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    bool bMouseHeld;
		
    //Fluid
    ofxFluid fluid;
    ofVec2f oldM;
    bool    bPaint, bObstacle, bBounding, bClear;
    ofFloatColor dyeColour;
    float dyeRadius;
    float dyeDensity;
    float dyeVelocityMult;
    bool bDrawFluid;
    
    //Blob detection
    ofxCvBlobFinder blobFinder;
    ofxCvColorImage inputImage;
    ofxCvGrayscaleImage inputImageGrey;
    ofPixels tempPixels;
    int threshold;
    bool bDrawBlobs;
    
    //Music control
    FluidPlayer fluidPlayer;
    
    //Kinect control
    FluidKinect fluidKinect;
    bool bDrawKinect;
    
    //Shaders
    ChannelBlender texBlender;
    
    //Saving frames
    ofFloatPixels velocityBufferPixels;
    ofFloatPixels smokeBufferPixels;
    vector<ofFloatPixels> velocityFilebuffer;
    vector<ofFloatPixels> smokeFilebuffer;
    bool areFilesSaved;
    void writeFramesToDisk();

    int savedFrameCount;
    bool isRecordingFrames;
    bool isWritingFrames;
    
    ofImage testImage;
    ofPixels testPixels;
   

};
