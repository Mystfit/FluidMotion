/*
 *  ofxOpticalFlow.cpp
 *  Created by lukasz karluk on 31/07/10.
 *
 */

#include "ofxOpticalFlowLK.h"

ofxOpticalFlowLK::ofxOpticalFlowLK() {
//	opticalFlowSize	= 5;
//	opticalFlowBlur = 7;
//	opticalFlowMin	= 0;
//	opticalFlowMax	= 0;
    
    opticalFlowSize	= 5;
	opticalFlowBlur = 7;
	opticalFlowMin	= 0;
	opticalFlowMax	= 0;
    
    opFlowVelX = NULL;
    opFlowVelY = NULL;
    
    bMirrorH = false;
    bMirrorV = false;
    
    bInitialised = false;
    bDrawLines = false;
}

ofxOpticalFlowLK::~ofxOpticalFlowLK() {
	destroy();
}

///////////////////////////////////////////
//	SETUP.
///////////////////////////////////////////

void ofxOpticalFlowLK::setup(const ofRectangle& size) {
	setup(size.width, size.height);
}

void ofxOpticalFlowLK::setup(int width, int height) {
	sizeSml.width = width;
	sizeSml.height = height;
	
	sizeLrg.width = width;		// assume input size is the same as optical flow size.
	sizeLrg.height = height;
	
	if(bInitialised) {
		destroy();
    }
	
	colrImgLrg.allocate(sizeLrg.width, sizeLrg.height);
	colrImgSml.allocate(sizeSml.width, sizeSml.height);
	greyImgLrg.allocate(sizeLrg.width, sizeLrg.height);
	greyImgSml.allocate(sizeSml.width, sizeSml.height);
	greyImgPrv.allocate(sizeSml.width, sizeSml.height);
	
	opFlowVelX = cvCreateImage(cvSize(sizeSml.width, sizeSml.height), IPL_DEPTH_32F, 1);
	opFlowVelY = cvCreateImage(cvSize(sizeSml.width, sizeSml.height), IPL_DEPTH_32F, 1);
    
    
    depthImageSml.allocate(sizeSml.width, sizeSml.height);
        
    velX.allocate(sizeSml.width, sizeSml.height);
    velY.allocate(sizeSml.width, sizeSml.height);
    
    velTex.allocate(sizeSml.width, sizeSml.height, OF_IMAGE_GRAYSCALE);
    
    xPix.allocate(sizeSml.width, sizeSml.height, 1);
    yPix.allocate(sizeSml.width, sizeSml.height, 1);
    zPix.allocate(sizeSml.width, sizeSml.height, 1);
    tempPixels.allocate(sizeSml.width, sizeSml.height, 3);

    //velTexY.allocate(sizeSml.width, sizeSml.height, OF_IMAGE_GRAYSCALE);
    //velPixels.allocate(sizeSml.width, sizeSml.height, OF_PIXELS_RGB);
    //velImg.allocate(sizeSml.width, sizeSml.height);
	
	reset();
	
	bInitialised = true;
}

void ofxOpticalFlowLK::setOpticalFlowSize(int value) {
    opticalFlowSize = value;
}

void ofxOpticalFlowLK::setOpticalFlowBlur(int value) {
    opticalFlowBlur = value;
}

void ofxOpticalFlowLK::reset() {
	colrImgLrg.set(0);
	colrImgSml.set(0);
	greyImgLrg.set(0);
	greyImgSml.set(0);
	greyImgPrv.set(0);

    cvSetZero(opFlowVelX);
    cvSetZero(opFlowVelY);
}

void ofxOpticalFlowLK::destroy() {
	colrImgLrg.clear();
	colrImgSml.clear();
	greyImgLrg.clear();
	greyImgSml.clear();
	greyImgPrv.clear();
	
    if(opFlowVelX) {
        cvReleaseImage(&opFlowVelX);
    }
    if(opFlowVelY) {
        cvReleaseImage(&opFlowVelY);
    }
}

///////////////////////////////////////////
//	UPDATE.
///////////////////////////////////////////

void ofxOpticalFlowLK::update(ofImage& source) {
	update(source.getPixels(), source.width, source.height, source.type);
}

void ofxOpticalFlowLK::update(ofxCvColorImage& source) {
	update(source.getPixels(), source.width, source.height, OF_IMAGE_COLOR);
}

void ofxOpticalFlowLK::update(ofxCvGrayscaleImage& source) {
	update(source.getPixels(), source.width, source.height, OF_IMAGE_GRAYSCALE);
}

void ofxOpticalFlowLK::update(ofxCvGrayscaleImage& source, ofxCvGrayscaleImage sourceDepth){
    depthImageSml = sourceDepth;
    update(source.getPixels(), source.width, source.height, OF_IMAGE_GRAYSCALE);
}


void ofxOpticalFlowLK::update(ofVideoPlayer& source) {
	update(source.getPixels(), source.width, source.height, OF_IMAGE_COLOR);	// assume colour image type.
}

void ofxOpticalFlowLK::update(ofVideoGrabber& source) {
	update(source.getPixels(), source.width, source.height, OF_IMAGE_COLOR);	// assume colour image type.
}

void ofxOpticalFlowLK::update(unsigned char* pixels, int width, int height, int imageType) {

	bool rightSize = (sizeSml.width == width && sizeSml.height == height);
	
	//-- making the input the right size for optical flow to work with.
	
	if(rightSize) {
		if(imageType == OF_IMAGE_COLOR) {
			colrImgSml.setFromPixels(pixels, sizeSml.width, sizeSml.height);
			greyImgSml.setFromColorImage(colrImgSml);
		} else if(imageType == OF_IMAGE_GRAYSCALE) {
			greyImgSml.setFromPixels(pixels, sizeSml.width, sizeSml.height);
		} else {
			return;		// wrong image type.
		}
	} else {

		bool sizeLrgChanged = (sizeLrg.width != width || sizeLrg.height != height);
		
		if(sizeLrgChanged) {		// size of input has changed since last update.
			sizeLrg.width = width;
			sizeLrg.height = height;
			
			colrImgLrg.clear();
			greyImgLrg.clear();
			
			colrImgLrg.allocate(sizeLrg.width, sizeLrg.height);
			greyImgLrg.allocate(sizeLrg.width, sizeLrg.height);
			
			colrImgLrg.set(0);
			greyImgLrg.set(0);
		}
		
		if(imageType == OF_IMAGE_COLOR) {
			colrImgLrg.setFromPixels(pixels, sizeLrg.width, sizeLrg.height);
			colrImgSml.scaleIntoMe(colrImgLrg);
			greyImgSml.setFromColorImage(colrImgSml);
		} else if(imageType == OF_IMAGE_GRAYSCALE) {
			greyImgLrg.setFromPixels(pixels, sizeLrg.width, sizeLrg.height);
			greyImgSml.scaleIntoMe(greyImgLrg);
		} else {
			return;		// wrong image type.
		}
	}
	
	if(bMirrorH || bMirrorV) {
		greyImgSml.mirror(bMirrorV, bMirrorH);
    }
	
    update(greyImgPrv.getCvImage(), greyImgSml.getCvImage(), opticalFlowSize, opticalFlowBlur);
	
	greyImgPrv = greyImgSml;
}

void ofxOpticalFlowLK::update(IplImage * previousImage, IplImage * currentImage, int opticalFlowSize, int opticalFlowBlur) {
    
    if((previousImage->width != currentImage->width) || (previousImage->height != currentImage->height)) {
        return; // images do not match.
    }
    
    int w = currentImage->width;
    int h = currentImage->height;

    if(opFlowVelX) {
        if((opFlowVelX->width != w) || (opFlowVelX->height != h)) {
            cvReleaseImage(&opFlowVelX);
            opFlowVelX = cvCreateImage(cvSize(w, h), IPL_DEPTH_32F, 1);
        }
    } else {
        opFlowVelX = cvCreateImage(cvSize(w, h), IPL_DEPTH_32F, 1);
    }
    
    if(opFlowVelY) {
        if((opFlowVelY->width != w) || (opFlowVelY->height != h)) {
            cvReleaseImage(&opFlowVelY);
            opFlowVelY = cvCreateImage(cvSize(w, h), IPL_DEPTH_32F, 1);
        }
    } else {
        opFlowVelY = cvCreateImage(cvSize(w, h), IPL_DEPTH_32F, 1);
    }
    
    int opFlowSize = opticalFlowSize;	// value must be 1, 3, 5, 7... etc.
    opFlowSize /= 2;
    opFlowSize *= 2;
    opFlowSize += 1;
	
	cvCalcOpticalFlowLK(previousImage, currentImage, cvSize(opFlowSize, opFlowSize), opFlowVelX, opFlowVelY);
	
	int opFlowBlur = opticalFlowBlur;	// value must be 1, 3, 5, 7... etc.
    opFlowBlur /= 2;
	opFlowBlur *= 2;
	opFlowBlur += 1;
	
	cvSmooth(opFlowVelX, opFlowVelX, CV_BLUR, opFlowBlur);
	cvSmooth(opFlowVelY, opFlowVelY, CV_BLUR, opFlowBlur);
    
    velX = opFlowVelX;
    velY = opFlowVelY;
            
    //ofLog(OF_LOG_NOTICE, ofToString(velX.getCvImage()->imageData[128*128]) );
        
    
    //xPix.setFromPixels(velX.getPixels(), sizeSml.width, sizeSml.height, 1);
    //yPix.setFromPixels(velY.getPixels(), sizeSml.width, sizeSml.height, 1);
    //zPix.setFromPixels(depthImageSml.getPixels(), sizeSml.width, sizeSml.height, 1);
    //tempPixels.setChannel(0, xPix);
    //tempPixels.setChannel(1, yPix);
    //tempPixels.setChannel(2, zPix);

    //velTex.setFromPixels(tempPixels);
    
    //velTexY.setFromPixels((unsigned char *)opFlowVelY->imageData, 320, 240, OF_IMAGE_GRAYSCALE);
    
//    ofFloatPixels tempPixels;
//    for(int i=0; i < 100; i++){
//        for(int j=0; j < 100; j++){
//            ofPoint vel = getVelAtPixel(i, j);
//            ofFloatColor velColor(vel.x, vel.y, 0.0f);
//            tempPixels.setColor(i, j, velColor);
//        }
//    }
//    
//    velTex.setFromPixels(tempPixels);
    
    //velTexX.setFromPixels(velX.getPixels(), 512, 512, OF_IMAGE_COLOR);
    //velTexY.setFromPixels(velY.getPixels(), 512, 512, OF_IMAGE_COLOR);

    //velTexX.loadData( velX.getPixels(), 320, 240, GL_RGB32F );
    //velTexY.loadData( velY.getPixels(), 320, 240, GL_RGB32F );
}

///////////////////////////////////////////
//	OP.FLOW VELOCITY GETTERS.
///////////////////////////////////////////




ofPoint ofxOpticalFlowLK::getVelAtNorm(float x, float y) {
	int px = x * (opFlowVelX->width - 1);
	int py = y * (opFlowVelX->height - 1);
    return getVelAtPixel(px, py);
}

ofPoint ofxOpticalFlowLK::getVelAtPixel(int x, int y) {
	x = ofClamp(x, 0, opFlowVelX->width - 1);
	y = ofClamp(y, 0, opFlowVelX->height - 1);
	
	ofPoint p;
	p.x = cvGetReal2D(opFlowVelX, y, x);
	p.y = cvGetReal2D(opFlowVelY, y, x);

	return p;
}

///////////////////////////////////////////
//	DRAW.
///////////////////////////////////////////

void ofxOpticalFlowLK::draw(int width, int height,  float lineScale, int res) {
    
//    if(bDrawLines)
//    {
        bool rightSize = (sizeSml.width == width && sizeSml.height == height);
        
        ofPoint vel;
    	
        for(int x=0; x<width; x+=res) {
            for(int y=0; y<height; y+=res) {
                if(rightSize) {
                    vel = getVelAtPixel(x, y);
                } else {
                    vel = getVelAtNorm(x / (float)width, y / (float)height);
                }
                
                if(vel.length() < 0) {  // smaller then 1 pixel, no point drawing.
                    continue;
                }
                
                //ofSetHexColor (ofColor(vel.x,vel.y,0.0f).getHex() );
                ofSetHexColor ( ofColor(255,255,255).getHex() );

                ofLine(x, y, x + vel.x * lineScale, y + vel.y * lineScale);
                
            }
        }
    //}
    velTex.draw(0.0f, 0.0f,320,240);

}

///////////////////////////////////////////
//	CONFIG.
///////////////////////////////////////////

void ofxOpticalFlowLK::setMirror(bool mirrorHorizontally, bool mirrorVertically) {
	bMirrorH = mirrorHorizontally;
	bMirrorV = mirrorVertically;
}
