//
//  FluidKinect.cpp
//  FluidMotion
//
//  Created by Byron Mallett on 10/10/12.
//
//

#include "FluidKinect.h"


FluidKinect::FluidKinect()
{
    
	isLive			= true;
	isTracking		= true;
	isFiltering		= false;
	isMasking		= true;
    
	nearThreshold = 500;
	farThreshold  = 1000;
    
    
	filterFactor = 0.1f;
    
    opticalBlur = 17;
    
    init();

}

void FluidKinect::init()
{
   
    
#if defined (TARGET_OSX) //|| defined(TARGET_LINUX) // only working on Mac/Linux at the moment (but on Linux you need to run as sudo...)
	hardware.setup();				// libusb direct control of motor, LED and accelerometers
    hardware.setLedOption(LED_GREEN); // turn off the led just for yacks (or for live installation/performances ;-)

#endif
    
	recordContext.setup();	// all nodes created by code -> NOT using the xml config file at all
	//recordContext.setupUsingXMLFile();
	recordDepth.setup(&recordContext);
    recordImage.setup(&recordContext);
    
    recordDepth.setDepthColoring(COLORING_GREY);

    recordUser.setup(&recordContext);
	recordUser.setSmoothing(filterFactor);				// built in openni skeleton smoothing...
	recordUser.setUseMaskPixels(isMasking);
	recordUser.setMaxNumberOfUsers(2);					// use this to set dynamic max number of users (NB: that a hard upper limit is defined by MAX_NUMBER_USERS in ofxUserGenerator)
    recordContext.toggleRegisterViewport();
	recordContext.toggleMirror();
    
    cameraImage.allocate(CAMERA_WIDTH, CAMERA_HEIGHT);
    cameraDepthImage.allocate(CAMERA_WIDTH, CAMERA_HEIGHT);

    blurImage.allocate(CAMERA_WIDTH, CAMERA_HEIGHT, GL_RGBA);
    maskImage.allocate(CAMERA_WIDTH, CAMERA_HEIGHT);
    
    depthPixels.allocate(CAMERA_WIDTH, CAMERA_HEIGHT, OF_PIXELS_RGB);
    maskTexture.allocate(CAMERA_WIDTH, CAMERA_HEIGHT , GL_LUMINANCE);
    maskPixels.allocate(CAMERA_WIDTH, CAMERA_HEIGHT, OF_PIXELS_RGBA);
    
    //Setup optical flow
    opFlowWidth = OPFLOW_WIDTH;
    opFlowHeight = OPFLOW_HEIGHT;
    opFlow.setup(ofRectangle(0,0, opFlowWidth, opFlowHeight ));
    opFlow.setOpticalFlowBlur(15);
    opFlow.setOpticalFlowSize(5);
    
}

void FluidKinect::update()
{
    hardware.update();
    if (isLive) {
        
        // update all nodes
		recordContext.update();
        recordDepth.update();
        recordImage.update();
        
        // update tracking/recording nodes
		if (isTracking){
            recordUser.update();
            hardware.setLedOption(LED_BLINK_RED_YELLOW);
        }
        
		if (isTracking && isMasking) {
//            cameraImage.setFromPixels(recordImage.getPixels(), recordUser.getWidth(), recordUser.getHeight());
//            maskImage.setFromPixels( recordUser.getUserPixels(), recordUser.getWidth(), recordUser.getHeight());
              maskTexture.loadData(recordUser.getUserPixels(),recordUser.getWidth(), recordUser.getHeight(), GL_LUMINANCE);
//                        
//            cameraImage.flagImageChanged();
//            maskImage.flagImageChanged();
//            
//            cameraImage.resize(320, 240);
//            maskImage.resize(320, 240);
            
            //Get blur image from shader instead!

//            blurImage = cameraImage;
//            blurImage *= maskImage;
//            opFlow.update(blurImage);
            
        }
    }
}

void FluidKinect::updateOpticalFlow(ofTexture & maskedKinect)
{
    maskedKinect.readToPixels(maskPixels);    
    blurImage.setFromPixels(maskPixels);
    blurImage.resize(opFlow.sizeSml.width, opFlow.sizeSml.height);
    blurImage.flagImageChanged();
    opFlow.update(blurImage);
}




void FluidKinect::draw()
{
    //recordDepth.draw(0,0,320,240);
    glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_DST_COLOR);
    blurImage.draw(0,0);
    opFlow.draw(OPFLOW_WIDTH, OPFLOW_HEIGHT);
    glDisable(GL_BLEND);
    
    if (isTracking){
        recordUser.draw();
    }
    
    glPushMatrix();
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
	allUserMasks.draw(CAMERA_WIDTH, 0, CAMERA_WIDTH, CAMERA_HEIGHT);
	glDisable(GL_BLEND);
    glPopMatrix();
}


void FluidKinect::keyPressed(int key){
    
	float smooth;
    
	switch (key) {
#ifdef TARGET_OSX // only working on Mac at the moment
		case 357: // up key
			hardware.setTiltAngle(hardware.tilt_angle++);
			break;
		case 359: // down key
			hardware.setTiltAngle(hardware.tilt_angle--);
			break;
#endif
		case 't':
		case 'T':
			isTracking = !isTracking;
			break;
        case 'm':
		case 'M':
			isMasking = !isMasking;
			recordUser.setUseMaskPixels(isMasking);
			break;
		case '9':
		case '(':
			smooth = recordUser.getSmoothing();
			if (smooth - 0.1f > 0.0f) {
				recordUser.setSmoothing(smooth - 0.1f);
			}
			break;
		case '0':
		case ')':
			smooth = recordUser.getSmoothing();
			if (smooth + 0.1f <= 1.0f) {
				recordUser.setSmoothing(smooth + 0.1f);
			}
			break;
		case '[':
            //case '{':
			if (filterFactor - 0.1f > 0.0f) {
				filterFactor = filterFactor - 0.1f;
            }
			break;
		case '>':
		case '.':
			farThreshold += 50;
			if (farThreshold > recordDepth.getMaxDepth()) farThreshold = recordDepth.getMaxDepth();
			break;
		case '<':
		case ',':
			farThreshold -= 50;
			if (farThreshold < 0) farThreshold = 0;
			break;
            
		case '+':
		case '=':
			nearThreshold += 50;
			if (nearThreshold > recordDepth.getMaxDepth()) nearThreshold = recordDepth.getMaxDepth();
			break;
            
		case '-':
		case '_':
			nearThreshold -= 50;
			if (nearThreshold < 0) nearThreshold = 0;
			break;
		case 'r':
			recordContext.toggleRegisterViewport();
			break;
		default:
			break;
	}
}
