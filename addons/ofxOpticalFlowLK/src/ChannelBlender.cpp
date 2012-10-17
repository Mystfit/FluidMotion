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
     
        float blueChan;
        
        void main(){
           
            vec2 st = gl_TexCoord[0].st;
            
//            blueChan = texture2DRect(blendB,st).b;
//            if(blueChan == 0.0){
//                blueChan = 1.0;
//            };
            //gl_FragColor = vec4(1.0,0.0,0.0,1.0);

            vec4 colour = vec4(texture2DRect(blendR,st).r, texture2DRect(blendG,st).r, 1.0 - texture2DRect(blendB,st).r, 1.0);
            gl_FragColor = colour * texture2DRect(mask,st);
        }
         
    );
    
    string fragVelocityBlendShader = STRINGIFY
    (
     uniform sampler2DRect    velocity;
     uniform sampler2DRect    depth;
     uniform sampler2DRect    mask;
     
     float blueChan;
     
     void main(){
         
         vec2 st = gl_TexCoord[0].st;
         
         //            blueChan = texture2DRect(blendB,st).b;
         //            if(blueChan == 0.0){
         //                blueChan = 1.0;
         //            };
         //gl_FragColor = vec4(1.0,0.0,0.0,1.0);
         
         vec4 colour = vec4(texture2DRect(velocity,st).rg, 1.0 - texture2DRect(depth,st).r, 1.0);
         gl_FragColor = colour * texture2DRect(mask,st);
     }
     
     );

    
    velocityBlendShader.unload();
    velocityBlendShader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragVelocityBlendShader);
    bFine = velocityBlendShader.linkProgram();
    
    
}

void ChannelBlender::allocate(int w, int h)
{
    width = w;
    height = h;
    blendBuffer.allocate(width, height, GL_RGB32F_ARB);
    
}

void ChannelBlender::update(ofTexture & velocityTex, ofTexture & depthTex, ofTexture & mask){
    blendBuffer.begin();
    blendShader.begin();
    blendShader.setUniformTexture("velocity", velocityTex, 0);
    blendShader.setUniformTexture("depth", depthTex, 1);
    blendShader.setUniformTexture("mask", mask, 2);
    
    renderFrame(width,height);
    
    blendShader.end();
    blendBuffer.end();
}


void ChannelBlender::update(ofTexture & redChan, ofTexture & greenChan, ofTexture & blueChan, ofTexture & mask)
{
    blendBuffer.begin();
    blendShader.begin();
    blendShader.setUniformTexture("blendR", redChan, 0);
    blendShader.setUniformTexture("blendG", greenChan, 1);
    blendShader.setUniformTexture("blendB", blueChan, 2);
    blendShader.setUniformTexture("mask", mask, 3);

    renderFrame(width,height);

    blendShader.end();
    blendBuffer.end();
}

void ChannelBlender::draw(int x, int y){
    blendBuffer.draw(x, y,640,480);
}