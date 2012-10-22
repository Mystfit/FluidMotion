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
#include "ofxCvComplexBlob.h"
#include "FluidNote.h"

using namespace std;

struct InstrumentParameter{
    int noteType;
    int channel;
    int source;
    int value;      //optional -- Used for CC noteOns/noteOffs
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
    
    void createCC(int ccName, int ccValue, ofPoint coords, float area);
    void createNote(string note, ofPoint coords, float area);
    void createNoteFromBlob(ofxCvComplexBlob blob);
    
    void addNote(FluidNote note);
    void removeNote(FluidNote note);
    void addparam(InstrumentParameter param){ params.push_back(param); };
    
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
