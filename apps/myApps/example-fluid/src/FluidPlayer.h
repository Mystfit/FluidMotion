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

class FluidPlayer : public ofxMidiListener {
public:
    FluidPlayer();
    
    void musicTick();
    void update();
    void updateNotes(vector<ofxCvComplexBlob> blobs);
    int getNoteIndexFromBlob(ofxCvComplexBlob blob);
    
    void newMidiMessage(ofxMidiMessage& eventArgs);
    
    float getBpm(){ return bpm; };
    
    
private:
    FluidInstrument m_activeInstrument;
    
    //MIDI
    ofxMidiIn midiIn;
    ofxMidiOut midiOut;
    unsigned int currentPgm;
    int synthChan, effectsChan, channel;
	int note, velocity;
	int pan, bend, touch, polytouch;
    int xPadSynth, yPadSynth, xPadEffects, yPadEffects;
    bool bIsPlaying;
    
    //Clock parameters
    int clockPastTime, clockTick, clockPeriod, beatCount, firstBeatMs, clockPeriodValue, BITPERMIN;
    float bpm;
    bool isBEAT;
    ofxMidiMessage lastMessage;

};

#endif /* defined(__FluidMotion__FluidPlayer__) */
