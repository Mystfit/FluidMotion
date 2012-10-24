//
//  FluidInstrument.h
//  FluidMotion
//
//  Created by Byron Mallett on 8/10/12.
//
//

#ifndef __FluidMotion__FluidInstrument__
#define __FluidMotion__FluidInstrument__


#include <iostream>
#include <vector>
#include <map>
#include <string>

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxCvBlobFinder.h"
#include "FluidNote.h"
#include "MusicMapper.h"


using namespace std;

struct InstrumentParameter{
    int noteType;
    int channel;
    int source;
    int value;      //optional -- Used for CC noteOns/noteOffs
    int upperNoteRange;    //optional -- used for upper note for instrument note mappings
    int lowerNoteRange;    //optional -- used for lower note for instrument note mappings
};

enum instrumentSourceTypes{
    INSTRUMENT_SOURCE_BLOBX = 1,
    INSTRUMENT_SOURCE_BLOBY,
    INSTRUMENT_SOURCE_AREA,
    INSTRUMENT_SOURCE_CURVATURE,
    INSTRUMENT_SOURCE_INTENSITY,
    INSTRUMENT_SOURCE_CCNOTEON,
    INSTRUMENT_SOURCE_CCNOTEOFF
};
enum instrumentNoteTypes{ INSTRUMENT_TYPE_POLYPHONIC = 0, INSTRUMENT_TYPE_MONOPHONIC };
enum instrumentNoteMappings{ INSTRUMENT_PLAYS_NOTES = 0, INSTRUMENT_PLAYS_CC };


class FluidInstrument {
public:
    FluidInstrument();
    FluidInstrument(string _name, string _device, int _channel, int _program, int _noteType, int _noteMapping);
    
    //Instrument parameters
    string name;                        // Instrument name
    string device;                      // Midi port instrument is located on
    int channel;                        // Midi channel to play on
    int program;                        // Midi program for instrument voice
    int noteType;                       // Polyphonic / monophonic instrument definition
    int noteMapping;                    // Notes or CC mappings
    bool usesCCNoteTriggers;              // Instrument requires noteOn/noteOff messages to be sent

    void setID(int instrumentId){ m_instrumentId = instrumentId; };
    
    vector<FluidNote> createNotesFromBlobParameters(BlobParam blobParameter);
    
    void addNote(FluidNote note);
    void removeNote(FluidNote note);
    
    int lerpNote(float value, int upper, int lower){
        int result;
        float calc = (float)lower + value*((float)upper - value);
        result = calc;
        return result;
    };
    
    void addparam(InstrumentParameter param){ params.push_back(param); };
    
    float blobParamValueFromSource(BlobParam blobParam, int source);
    int getParamSourceFromString(string source);
    InstrumentParameter getParamFromSource(int source);
    
    void startInstrument();
    void stopInstrument();
    
    vector<FluidNote> activeNotes;
    vector<InstrumentParameter> params;
    
 
private:
    int m_instrumentId;
    int m_noteIdCounter;
    
};

#endif /* defined(__FluidMotion__FluidInstrument__) */
