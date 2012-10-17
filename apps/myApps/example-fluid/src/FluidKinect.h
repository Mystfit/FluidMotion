//
//  FluidKinect.h
//  FluidMotion
//
//  Created by Byron Mallett on 10/10/12.
//
//

#ifndef __FluidMotion__FluidKinect__
#define __FluidMotion__FluidKinect__

#include <iostream>

#include "ofMain.h"
#include "ofxOpenNI.h"
#include "ofxOpenCv.h"
#include "ofxOpticalFlowLK.h"


class FluidKinect {
    
    
    //Kinect parameters
    //------------------------------
private:
    ofxOpenNIContext	recordContext;
	ofxDepthGenerator	recordDepth;
#ifdef USE_IR
	ofxIRGenerator		recordImage;
#else
	ofxImageGenerator	recordImage;
#endif
	ofxUserGenerator	recordUser;
    
#if defined (TARGET_OSX)
	ofxHardwareDriver	hardware;
#endif
    
	bool				isLive, isTracking, isMasking, isFiltering;
	int					nearThreshold, farThreshold;
	float				filterFactor;
    ofImage				allUserMasks, user1Mask, user2Mask, depthRangeMask;
    
    //Optical flow
    //------------------------------
    ofxCvColorImage cameraImage;
    ofxCvColorImage cameraDepthImage;
    ofxCvGrayscaleImage cameraDepthImageGrey;

    ofxCvGrayscaleImage blurImage;
    ofxCvGrayscaleImage maskImage;
    ofPixels depthPixels;
    ofTexture maskTexture;
    int opticalBlur;
    
public:
    FluidKinect();
    void init();
    void update();
    void draw();
    void keyPressed(int key);
    
    ofxOpticalFlowLK opFlow;
    ofFbo velFbo;

    
    ofTexture & getVelocityTexture(int w, int h);
    
    ofTexture & getDepthTexture(){ return recordDepth.depth_texture; };
    ofTexture & getMaskTexture(){ return maskTexture; };


};

    
#endif /* defined(__FluidMotion__FluidKinect__) */
    




