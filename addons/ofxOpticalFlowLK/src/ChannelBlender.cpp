//
//  ChannelBlender.cpp
//  FluidMotion
//
//  Created by Byron Mallett on 16/10/12.
//
//

#include "ChannelBlender.h"


ChannelBlender::ChannelBlender()
{
    string fragBlendShader = STRINGIFY
    (
        uniform sampler2DRect    blendR;
        uniform sampler2DRect    blendG;
        uniform sampler2DRect    blendB;
        uniform sampler2DRect    mask;
        uniform float            maxDist;
        uniform float            minDist;
     
        float clampedDepth;
             
        void main(){
           
            vec2 st = gl_TexCoord[0].st;
            clampedDepth = smoothstep(minDist, maxDist, texture2DRect(blendB, st).r);
            vec4 colour = vec4(texture2DRect(blendR,st).r, texture2DRect(blendG,st).r, clampedDepth, 1.0);
            gl_FragColor = colour * texture2DRect(mask,st);
        }
         
    );
    
    blendShader.unload();
    blendShader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragBlendShader);
    bFine = blendShader.linkProgram();
    
    string fragMaskShader = STRINGIFY
    (
     uniform sampler2DRect    camera;
     uniform sampler2DRect    mask;
        
     void main(){
         
         vec2 st = gl_TexCoord[0].st;
         float gray = dot(texture2DRect(camera,st).rgb, vec3(0.299, 0.587, 0.114));
         gl_FragColor = vec4(gray * texture2DRect(mask,st).r, gray * texture2DRect(mask,st).r, gray * texture2DRect(mask,st).r, 1.0);
     }
     
     );
    
    kinectMaskShader.unload();
    kinectMaskShader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragMaskShader);
    bFine = kinectMaskShader.linkProgram();
    
    
}

void ChannelBlender::allocate(int w, int h)
{
    width = w;
    height = h;
    blendBuffer.allocate(256, 256, GL_RGBA32F_ARB);
    kinectBuffer.allocate(320, 240, GL_RGB);
}

ofFbo * ChannelBlender::updateBlender(ofTexture & redChan, ofTexture & greenChan, ofTexture & blueChan, ofTexture & mask, float maxDist, float minDist, int w, int h)
{
    blendBuffer.dst->begin();
    blendShader.begin();
    blendShader.setUniformTexture("blendR", redChan, 0);
    blendShader.setUniformTexture("blendG", greenChan, 1);
    blendShader.setUniformTexture("blendB", blueChan, 2);
    blendShader.setUniformTexture("mask", mask, 3);
    blendShader.setUniform1f("maxDist", maxDist);
    blendShader.setUniform1f("minDist", minDist);

    renderFrame(w,h);

    blendShader.end();
    blendBuffer.dst->end();
    
    return blendBuffer.dst;
}

ofFbo * ChannelBlender::updateKinectMasker(ofTexture & colourCamera, ofTexture & mask, int w, int h){
    
    kinectBuffer.dst->begin();
    kinectMaskShader.begin();
    kinectMaskShader.setUniformTexture("camera", colourCamera, 0);
    kinectMaskShader.setUniformTexture("mask", mask, 2);
    
    renderFrame(w,h);
    
    kinectMaskShader.end();
    kinectBuffer.dst->end();
}
