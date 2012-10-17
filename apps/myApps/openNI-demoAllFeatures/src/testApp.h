#ifndef _TEST_APP
#define _TEST_APP

//#define USE_IR // Uncomment this to use infra red instead of RGB cam...
#include "ofxOpenNI.h"
#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxOpenCv.h"
#include "ofxOpticalFlowLK.h"


#define OPTICAL_FLOW_DEFAULT_WIDTH		320
#define OPTICAL_FLOW_DEFAULT_HEIGHT		240


class testApp : public ofBaseApp{

public:
    //Openframeworks methods
    //------------------------------

	void setup();
	void update();
	void draw();
    void exit();

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);

    
    //Kinect methods
    //------------------------------
	void	setupRecording(string _filename = "");
	void	setupPlayback(string _filename);
    void	drawMasks();
	void	drawPointCloud(ofxUserGenerator * user_generator, int userID);
    
    
    //Optical flow methods
    //------------------------------
//    void    setupOpticalFlow();
//    void    destroyOpticalFLow();
    
    ofxCvGrayscaleImage blurImage;
    ofxCvGrayscaleImage depthProcessingImage;


    
    //Kinect parameters
    //------------------------------

	ofxOpenNIContext	recordContext, playContext;
	ofxDepthGenerator	recordDepth, playDepth;
#ifdef USE_IR
	ofxIRGenerator		recordImage, playImage;
#else
	ofxImageGenerator	recordImage, playImage;
#endif
	ofxHandGenerator	recordHandTracker, playHandTracker;
	ofxUserGenerator	recordUser, playUser;
	ofxOpenNIRecorder	oniRecorder;

#if defined (TARGET_OSX) //|| defined(TARGET_LINUX) // only working on Mac/Linux at the moment (but on Linux you need to run as sudo...)
	ofxHardwareDriver	hardware;
#endif
    
	bool				isLive, isTracking, isRecording, isCloud, isCPBkgnd, isMasking;
	bool				isTrackingHands, isFiltering;
	int					nearThreshold, farThreshold;
	int					pointCloudRotationY;
	float				filterFactor;
    string              generateFileName();
    ofImage				allUserMasks, user1Mask, user2Mask, depthRangeMask;

        
    //Midi parameters
    //------------------------------
    ofxMidiOut midiOut;
    
    unsigned int currentPgm;
    int synthChan, effectsChan, channel;
	int note, velocity;
	int pan, bend, touch, polytouch;
    int xPadSynth, yPadSynth, xPadEffects, yPadEffects;
    
    bool isPlaying;
    
    ofxOpticalFlowLK opFlow;
    
    
    //Optical flow parameters
    //------------------------------
//    ofPoint getVelAtNorm(float x, float y);
//	ofPoint getVelAtPixel(int x, int y);
//	ofRectangle sizeSml;
//	ofRectangle sizeLrg;
//    ofxCvColorImage colrImgLrg;		// full scale color image.
//	ofxCvColorImage	colrImgSml;		// full scale color image.
//	ofxCvGrayscaleImage	greyImgLrg;	// full scale grey image.
//	ofxCvGrayscaleImage	greyImgSml;	// scaled down grey image.
//	ofxCvGrayscaleImage	greyImgPrv;	// scaled down grey image - copy of previous frame.
//	IplImage * opFlowVelX;          // optical flow in the x direction.
//	IplImage * opFlowVelY;          // optical flow in the y direction.
//    
//    bool bInitialised;
//    bool bMirrorH;
//	bool bMirrorV;
//	
//	int opticalFlowSize;
//	int opticalFlowBlur;
//	float opticalFlowMin;
//	float opticalFlowMax;
};

#endif
