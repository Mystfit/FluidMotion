#include "FluidMotionApp.h"


//--------------------------------------------------------------
void FluidMotionApp::setup(){
    
    //ofEnableAlphaBlending();
	ofSetWindowShape(512, 512);
    ofSetLogLevel(OF_LOG_NOTICE);
    ofEnableArbTex();
    
    // Initial Allocation
    //
    fluid.allocate(LOWREZ_FLUID_SIZE, LOWREZ_FLUID_SIZE, 1, 1.0f);
    fluidPlayback.allocate(HIREZ_FLUID_SIZE, HIREZ_FLUID_SIZE, 0.5f);
    inputImage.allocate(LOWREZ_FLUID_SIZE, LOWREZ_FLUID_SIZE);
    inputImageGrey.allocate(LOWREZ_FLUID_SIZE, LOWREZ_FLUID_SIZE);
    tempPixels.allocate(LOWREZ_FLUID_SIZE,LOWREZ_FLUID_SIZE,3);
    
    texBlender.allocate(CAMERA_WIDTH, CAMERA_HEIGHT, LOWREZ_FLUID_SIZE, LOWREZ_FLUID_SIZE);
    kinectSavedOpflowBlender.allocate(CAMERA_WIDTH, CAMERA_HEIGHT, HIREZ_FLUID_SIZE, HIREZ_FLUID_SIZE);
    
    //Buffer for saving images
    velocityBufferPixels.allocate(HIREZ_FLUID_SIZE, HIREZ_FLUID_SIZE, 3);
    smokeBufferPixels.allocate(HIREZ_FLUID_SIZE, HIREZ_FLUID_SIZE, 3);
    playbackPixels.allocate(HIREZ_FLUID_SIZE, HIREZ_FLUID_SIZE, 3);
    playbackFrame = 0;
        
    threshold = 30;
    dyeColour.set(1.0f,1.0f,1.0f);
    dyeRadius = 10.f;
    dyeDensity = 0.1f;
    dyeVelocityMult = 1.0f;
    
    //Kinect properties
    depthActivationEnd = 0.1f;
    depthActivationStart = 0.06f;
    
    // Seting the gravity set up & injecting the background image
    fluid.setDissipation(0.97);
    fluid.setVelocityDissipation(0.97);
    fluid.setDyeColour(ofVec3f(0.2f, 0.2f, 1.0f));
    
    fluidPlayback.setDissipation(0.97);
    fluidPlayback.setVelocityDissipation(0.97);
    fluidPlayback.setDyeColour(ofVec3f(0.2f, 0.2f, 1.0f));
    
    bDrawKinect = true;
    bDrawFluid = true;
    bDrawBlobs = false;
    bCalculateBlobs = true;
    
    fluidPlayer.startPerformance();
    
    //fluid.addConstantForce(ofPoint(256*0.5f,256*0.5f), ofPoint(0,0), ofFloatColor(1.0f,1.0f,1.0f), 20.0f);
    
}


//--------------------------------------------------------------
void FluidMotionApp::update(){
    
    // Adding temporal Force
    //
    if(bMouseHeld){
        ofPoint m = ofPoint(mouseX,mouseY);
        ofPoint d = (m - oldM)*2.0f;
        oldM = m;
        ofPoint c = ofPoint(512*0.5, 512*0.5) - m;
        c.normalize();
            
        fluid.addTemporalForce(m, d*dyeVelocityMult, dyeColour, dyeRadius,100.0f,0.5f);
    }
    
    
    //  Update
    //--------
    
    if(!isPlayingBackFrames)
    {
        
        //Update kinect cameras
        fluidKinect.update();
        
        //Mask kinect camera
        texBlender.updateKinectMasker(fluidKinect.getCameraTexture(), fluidKinect.getMaskTexture(), CAMERA_WIDTH, CAMERA_HEIGHT);
        
        //Update optical flow with masked kinect image
        fluidKinect.updateOpticalFlow(texBlender.kinectBuffer.dst->getTextureReference());
        
        //Blend optical flow velocites and depth camera
        texBlender.updateBlender( fluidKinect.opFlow.velTexX.getTextureReference(), fluidKinect.opFlow.velTexY.getTextureReference(), fluidKinect.getDepthTexture(), fluidKinect.getMaskTexture(), depthActivationStart, depthActivationEnd);
        
        if(isRecordingFrames){
            //texBlender.blendBuffer.dst->readToPixels(playbackPixels);
          kinectSavedOpflowBlender.updateBlender( fluidKinect.opFlow.velTexX.getTextureReference(), fluidKinect.opFlow.velTexY.getTextureReference(), fluidKinect.getDepthTexture(), fluidKinect.getMaskTexture(), depthActivationStart, depthActivationEnd)->readToPixels(playbackPixels);
            
            fluidInputTextures.push_back( playbackPixels );
            ofLog(OF_LOG_NOTICE, ofToString(fluidInputTextures.size()));
        }
        
        //Send camera velocities to fluid simulation
        fluid.setExternalVelocity(  texBlender.blendBuffer.dst->getTextureReference() );
        fluid.update();    
    
        //Do blob detection on fluid simulation output
        if(bCalculateBlobs){
            fluid.getPingPong().src->readToPixels(tempPixels);
            inputImage.setFromPixels(tempPixels.getPixels(),  fluid.getPingPong().src->getWidth(), fluid.getPingPong().src->getHeight());
            inputImage.flagImageChanged();
            inputImageGrey = inputImage;
            inputImageGrey.blurHeavily();
            //inputImageGrey.blurGaussian(7);
            inputImageGrey.threshold(40);
            inputImageGrey.flagImageChanged();
            blobFinder.findBlobs(inputImageGrey, false);
            
            //Generate notes from fluid blob output
            blobFinder.matchExistingBlobs();
            
            if(blobFinder.blobParams.size() > 0)
                fluidPlayer.blobsToNotes(blobFinder.blobParams);
            else
                fluidPlayer.clearAllNotes();
        }
    }
    
    //Replay fluid from saved optical flow source
    else
    {
        //Send camera velocities to fluid simulation
        fluidPlayback.setExternalVelocity( ofImage(fluidInputTextures[playbackFrame]).getTextureReference() );
        fluidPlayback.update();
       
        fluidPlayback.getVelocityBuffer().src->readToPixels(velocityBufferPixels);
        fluidPlayback.getPingPong().src->readToPixels(smokeBufferPixels);
            
        ofSaveImage(velocityBufferPixels, "frames/velocity_" + ofGetTimestampString() + "_" + ofToString(playbackFrame) + ".exr" );
        ofSaveImage(smokeBufferPixels, "frames/smoke_" + ofGetTimestampString() + "_" + ofToString(playbackFrame) + ".exr" );

        if(playbackFrame == fluidInputTextures.size() - 1)
            isPlayingBackFrames = false;

        playbackFrame++;
    }

    ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void FluidMotionApp::draw(){
    ofBackground(0);
    
    if(fluidPlayer.isBeatDirty()){
        ofFloatColor(1.0f, 0.0f, 0.0f);
        ofCircle(10.0f, 20.0f, 10.0f);
        fluidPlayer.setBeatClean();
    }
    
    
    glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
    
    if(bDrawFluid)
        (!isPlayingBackFrames) ? fluid.draw(0,0,ofGetScreenHeight(),ofGetScreenHeight()) : fluidPlayback.draw(0,0,ofGetScreenHeight(),ofGetScreenHeight());;
    if(bDrawBlobs) blobFinder.draw(0,0,ofGetScreenHeight(),ofGetScreenHeight());
    
    ofSetHexColor(0xFFFFFF);
    
    if(!fluidPlayer.isPlaying)
        ofDrawBitmapString("Waiting for play message from Live",10.0f,10.0f);
    else if(isWritingFrames)
        ofDrawBitmapString("Writing frames to disk...",10.0f,10.0f);
    else if(fluidPlayer.getBpm() > 0)
        ofDrawBitmapString(ofToString(fluidPlayer.getBpm()) + "bpm", 10.0f,10.0f);

    
    glDisable(GL_BLEND);
}

//--------------------------------------------------------------
void FluidMotionApp::keyPressed(int key){
    if( key == 'p')
        bPaint = !bPaint;
    if( key == 'o')
        bDrawFluid = !bDrawFluid;
    if( key == 'i')
         bDrawKinect = !bDrawKinect;
    if( key == 'u')
        bDrawBlobs = ! bDrawBlobs;
    if( key == 'b')
        bCalculateBlobs = ! bCalculateBlobs;
    if( key == 'y')
        fluidKinect.opFlow.bDrawLines != fluidKinect.opFlow.bDrawLines;
    if( key == 'v')
        fluid.bDrawVelocity != fluid.bDrawVelocity;
    if( key == 'c')
        fluid.bDrawPressure != fluid.bDrawPressure;
    if( key =='r'){
        if(!isRecordingFrames) {
            isRecordingFrames = true;
        } else if(isRecordingFrames && !isPlayingBackFrames){
            isRecordingFrames = false;
            isPlayingBackFrames = true;
        }
    }
    
    //Forward keypress to kinect
    fluidKinect.keyPressed(key);
}


void FluidMotionApp::recordHirezSim()
{
    isPlayingBackFrames = true;
    playbackFrame = 0;
}


//--------------------------------------------------------------
void FluidMotionApp::keyReleased(int key){

}

//--------------------------------------------------------------
void FluidMotionApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void FluidMotionApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void FluidMotionApp::mousePressed(int x, int y, int button){
    bMouseHeld = true;
    if(bPaint){
        fluid.bIsAddingDye = false;
        dyeRadius = 25.0f;
        dyeDensity = 0.1f;
        dyeVelocityMult = 0.3f;

    }else{
        fluid.bIsAddingDye = true;
        dyeRadius = 10.0f;
        dyeDensity = 0.5f;
        dyeVelocityMult = 0.8f;
    }
}

//--------------------------------------------------------------
void FluidMotionApp::mouseReleased(int x, int y, int button){
    
    bMouseHeld = false;


}

//--------------------------------------------------------------
void FluidMotionApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void FluidMotionApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void FluidMotionApp::dragEvent(ofDragInfo dragInfo){ 

}

