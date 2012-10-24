//
//  FluidNote.h
//  FluidMotion
//
//  Created by Byron Mallett on 8/10/12.
//
//

#ifndef __FluidMotion__FluidNote__
#define __FluidMotion__FluidNote__

#include <iostream>
#include <string>
#include "ofMain.h"

enum status{ON =0, HOLD, OFF};

#define CC_MAX_VALUE 127
#define CC_MIN_VALUE 0

struct NoteParameters{
    float area;
    ofPoint fluidPosition;
};

using namespace std;



class FluidNote {
public:
    FluidNote();
    FluidNote(int noteId, int instrumentId, int type);
    
    void setType(int noteType){m_type = noteType;};
    int getType(){ return m_type; };
    
    int getStatus(){ return m_noteStatus; };
    void setStatus(int noteStatus) { m_noteStatus = noteStatus; };
    
    string getNote(){ return m_note; };
    void setNote(string note);
    
    int getCCName(){ return m_ccName; };
    void setCCName(int ccName){ m_ccName = ccName; };
    
    int getCCValue(){ return m_ccValue; };
    void setCCValue(int ccValue);
    
    NoteParameters getParams(){ return noteParams; };
    void setParams(float area, ofPoint fluidPositon)
    {
        noteParams.area = area;
        noteParams.fluidPosition = fluidPositon;
    };
    
    int getNoteType(){ return m_type;};
    int getNoteInstrument(){ return m_instrumentId;};
    int getNoteId(){ return m_noteId;};
    
    void cleanNote(){ bNoteChanged = false; };
    
protected:
    int m_type;
    string m_note;
    int m_ccName;
    int m_ccValue;
    int m_noteId;
    int m_instrumentId;
    int m_noteStatus;
    
    NoteParameters noteParams;
    
    bool bNoteChanged;
};

#endif /* defined(__FluidMotion__FluidNote__) */
