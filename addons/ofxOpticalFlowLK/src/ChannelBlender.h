//
//  ChannelBlender.h
//  FluidMotion
//
//  Created by Byron Mallett on 16/10/12.
//
//

#ifndef __FluidMotion__ChannelBlender__
#define __FluidMotion__ChannelBlender__

#include <iostream>
#include "ofMain.h"
#include "ofxFXObject.h"

class ChannelBlender : public ofxFXObject{
public:
    
    ChannelBlender();
    void allocate(int w, int h);
    ofFbo * updateBlender(ofTexture & redChan, ofTexture & greenChan, ofTexture & blueChan, ofTexture & mask, float maxDist, float minDist, int w, int h);
    ofFbo * updateKinectMasker(ofTexture & colourCamera, ofTexture & mask, int w, int h);
    
    ofShader blendShader;
    ofShader kinectMaskShader;

    ofxSwapBuffer blendBuffer;
    ofxSwapBuffer kinectBuffer;
};

#endif /* defined(__FluidMotion__ChannelBlender__) */
