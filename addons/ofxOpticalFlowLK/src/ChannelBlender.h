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
    void update(ofTexture & velocityTex, ofTexture & depthTex, ofTexture & mask);
    void update(ofTexture & redChan, ofTexture & greenChan, ofTexture & blueChan, ofTexture & mask);
    void draw(int x, int y);
    
    ofShader blendShader;
    ofShader velocityBlendShader;

    ofFbo blendBuffer;
};

#endif /* defined(__FluidMotion__ChannelBlender__) */
