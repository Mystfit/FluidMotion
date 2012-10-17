#include "FluidMotionApp.h"

//--------------------------------------------------------------
void FluidMotionApp::setup(){
    
    //ofEnableAlphaBlending();
	ofSetWindowShape(512, 512);
    
    ofSetLogLevel(OF_LOG_ERROR);
    
    // Initial Allocation
    //
    fluid.allocate(256, 256, 1);
    inputImage.allocate(256, 256);
    inputImageGrey.allocate(256, 256);
    tempPixels.allocate(256,256,3);
    texBlender.allocate(320, 240);
        
    threshold = 50;
    dyeColour.set(1.0f,1.0f,1.0f);
    dyeRadius = 10.f;
    dyeDensity = 0.1f;
    dyeVelocityMult = 1.0f;
    
    // Seting the gravity set up & injecting the background image
    //fluid.setGravity(ofVec2f(0.0,-0.098));
    fluid.setDissipation(0.99);
    fluid.setVelocityDissipation(0.85);
    
    bDrawKinect = true;
    bDrawFluid = false;
    bDrawBlobs = false;
    
    fluid.addConstantForce(ofPoint(256*0.5,256*0.85), ofPoint(0,-25.0f), ofFloatColor(1.0f,1.0f,1.0f), 40.f);
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
    
    fluidKinect.update();
    texBlender.update( fluidKinect.opFlow.velX.getTextureReference(), fluidKinect.opFlow.velY.getTextureReference(), fluidKinect.getDepthTexture(), fluidKinect.getMaskTexture());
    
    fluid.setExternalVelocity(  texBlender.blendBuffer.getTextureReference() );
    fluid.update();
    fluid.getPingPong().src->readToPixels(tempPixels);
            
    inputImage.setFromPixels(tempPixels.getPixels(),256, 256);
    inputImage.flagImageChanged();
    inputImageGrey = inputImage;
    
    inputImageGrey.blurHeavily();
    inputImageGrey.threshold(threshold);
    //inputImageGrey.invert();
    
    blobFinder.findBlobs(inputImageGrey, false);
    
    fluidPlayer.updateNotes(blobFinder.blobz);
    fluidPlayer.update();

    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void FluidMotionApp::draw(){
    ofBackground(0);
    
    //glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ZERO);
    
    fluidKinect.opFlow.velX.draw(0.0f, 0.0f);
    fluidKinect.opFlow.velY.draw(0.0f, 0.0f);
    
    texBlender.draw(320,0);

    //if(bDrawKinect) fluidKinect.draw();
    if(bDrawFluid) fluid.draw(0,0,512,512);
    //if(bDrawBlobs) blobFinder.draw(0,0,512,512);
    

    
    //glDisable(GL_BLEND);

    
    //fluidKinect.opFlow.velTexture.draw(0,0, 320, 240);
    //inputImage.draw(0,0,512,512);
    //inputImageGrey.draw(0,0,512,512);

}

//--------------------------------------------------------------
void FluidMotionApp::keyPressed(int key){
    if( key == 'p')
        bPaint = !bPaint;
    if( key == 'o')
        bDrawFluid = !bDrawFluid;
        //bObstacle = !bObstacle;
    if( key == 'i')
         bDrawKinect = !bDrawKinect;
        //bBounding = !bBounding;
    if( key == 'u')
        bDrawBlobs = ! bDrawBlobs;
        //bClear = !bClear;
    if( key == 'y')
        fluidKinect.opFlow.bDrawLines != fluidKinect.opFlow.bDrawLines;
    if( key == 'v')
        fluid.bDrawVelocity != fluid.bDrawVelocity;
    if( key == 'c')
        fluid.bDrawPressure != fluid.bDrawPressure;


    
    //Forward keypress to kinect
    fluidKinect.keyPressed(key);
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