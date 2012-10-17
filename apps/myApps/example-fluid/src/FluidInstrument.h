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
#include "FluidNote.h"

using namespace std;

struct InstrumentParameter{
    float paramValue;
    string paramName;
};

class FluidInstrument {
public:
    FluidInstrument();
    
    void setID(int instrumentId){ m_instrumentId = instrumentId; };
    
    void createCC(int ccName, int ccValue, ofPoint coords, float area);
    void createNote(string note, ofPoint coords, float area);
    void addNote(FluidNote note);
    void removeNote(FluidNote note);
    void addparam(InstrumentParameter param){ params.push_back(param); };
    
    vector<FluidNote> activeNotes;
    vector<InstrumentParameter> params;
private:
    int m_instrumentId;
    int m_noteIdCounter;
};

#endif /* defined(__FluidMotion__FluidInstrument__) */
