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
#include "ofxFXObject.h"
#include "ofMain.h"

class ChannelBlender : public ofxFXObject{
public:
    
    ChannelBlender();
    void allocate(int w, int h);
    void update(ofTexture & redChan, ofTexture & greenChan, ofTexture & blueChan, ofTexture & mask);
    void draw(int x, int y);
    
    ofShader blendShader;
    ofFbo blendBuffer;
};

#endif /* defined(__FluidMotion__ChannelBlender__) */
