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
        uniform float scaleX;
        uniform float scaleY;
        uniform float maskScaleX;
        uniform float maskScaleY;
     
        float clampedDepth;
             
        void main(){
           
            vec2 st = gl_TexCoord[0].st;
            vec2 stOffset = vec2(gl_TexCoord[0].s * scaleX, gl_TexCoord[0].t * scaleY);
            vec2 mask_stOffset = vec2(gl_TexCoord[0].s * maskScaleX, gl_TexCoord[0].t * maskScaleY);

            clampedDepth = smoothstep(minDist, maxDist, texture2DRect(blendB, mask_stOffset).r);
            vec4 colour = vec4(texture2DRect(blendR,stOffset).r, texture2DRect(blendG,stOffset).r, clampedDepth, 1.0);
            gl_FragColor = colour * texture2DRect(mask,mask_stOffset);
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

void ChannelBlender::allocate(int w, int h, int outWidth, int outHeight)
{
    width = w;
    height = h;
    blenderWidth = outWidth;
    blenderHeight = outHeight;
    blendBuffer.allocate(blenderWidth, blenderHeight, GL_RGBA32F_ARB);
    kinectBuffer.allocate(width, width, GL_RGB);
}

ofFbo * ChannelBlender::updateBlender(ofTexture & redChan, ofTexture & greenChan, ofTexture & blueChan, ofTexture & mask, float maxDist, float minDist)
{
    float scaleX = width*0.5 / blenderWidth;
    float scaleY = height*0.5 / blenderHeight * 0.9f;      //Guessing at resize offset, would like to track down where this is happening
    
    float maskScaleX = width / blenderWidth;
    float maskScaleY = height / blenderHeight;
    
    ofLog(OF_LOG_NOTICE, ofToString(redChan.getWidth()) + " -- " + ofToString(redChan.getHeight()));

    blendBuffer.dst->begin();
    blendShader.begin();
    blendShader.setUniformTexture("blendR", redChan, 0);
    blendShader.setUniformTexture("blendG", greenChan, 1);
    blendShader.setUniformTexture("blendB", blueChan, 2);
    blendShader.setUniformTexture("mask", mask, 3);
    blendShader.setUniform1f("maxDist", maxDist);
    blendShader.setUniform1f("minDist", minDist);
    blendShader.setUniform1f("scaleX", scaleX);
    blendShader.setUniform1f("scaleY", scaleY);
    blendShader.setUniform1f("maskScaleX", maskScaleX);
    blendShader.setUniform1f("maskScaleY", maskScaleY);

    renderFrame(blenderWidth,blenderHeight);

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
