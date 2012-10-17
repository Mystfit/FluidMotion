//
//  FluidPlayer.h
//  FluidMotion
//
//  Created by Byron Mallett on 9/10/12.
//
//

#ifndef __FluidMotion__FluidPlayer__
#define __FluidMotion__FluidPlayer__

#include <iostream>
#include "ofxMidi.h"
#include "FluidInstrument.h"
#include "ofxCvComplexBlob.h"

class FluidPlayer {
public:
    FluidPlayer();
    
    void update();
    void updateNotes(vector<ofxCvComplexBlob> blobs);
    int getNoteIndexFromBlob(ofxCvComplexBlob blob);

    
    
private:
    FluidInstrument m_activeInstrument;
    
    //MIDI
    ofxMidiOut midiOut;
    unsigned int currentPgm;
    int synthChan, effectsChan, channel;
	int note, velocity;
	int pan, bend, touch, polytouch;
    int xPadSynth, yPadSynth, xPadEffects, yPadEffects;
    bool bIsPlaying;

};

#endif /* defined(__FluidMotion__FluidPlayer__) */
