#pragma once

#include "ofMain.h"
#include "ofxFX.h"
#include "ofxCvBlobFinder.h"
#include "ofxMidi.h"
#include "ofxOpenNI.h"

#include "FluidPlayer.h"
#include "FluidKinect.h"
#include "ChannelBlender.h"

#define LOWREZ_FLUID_SIZE 256
#define HIREZ_FLUID_SIZE 1024

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
    ofxFluid fluidPlayback;

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
    bool bCalculateBlobs;
    
    //Music control
    FluidPlayer fluidPlayer;
    
    //Kinect control
    FluidKinect fluidKinect;
    bool bDrawKinect;
    float depthActivationStart, depthActivationEnd;
    void increaseFarDepth();
    void decreaseFarDepth();
    void increaseNearDepth();
    void decreaseNearDepth();
    void increaseThreshold();
    void decreaseThreshold();
    
    //Shaders
    ChannelBlender texBlender;
    
    //Saving frames
    ofFloatPixels velocityBufferPixels;
    ofFloatPixels smokeBufferPixels;
    vector<ofFloatPixels> velocityFilebuffer;
    vector<ofFloatPixels> smokeFilebuffer;
    vector<ofFloatPixels> fluidInputTextures;
    ChannelBlender kinectSavedOpflowBlender;
    ofFloatPixels playbackPixels;
    vector<ofVec3f> fluidRecordedColour;

    bool areFilesSaved;
    void writeFramesToDisk();
    void recordHirezSim();

    int playbackFrame;
    bool isRecordingFrames;
    bool isPlayingBackFrames;
    bool isWritingFrames;
    
    ofImage testImage;
    ofPixels testPixels;
};
