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
    FluidNote(int noteId, string instrumentName, int type);
    
    void setType(int noteType){m_type = noteType;};
    int getType(){ return m_type; };
    
    int getStatus(){ return m_noteStatus; };
    void setStatus(int noteStatus) { m_noteStatus = noteStatus; };
    
    int getSource(return m_source);
    void setSource(int source){m_source = source;};
      
    int getCCchan(){ return m_CCchan; };
    void setCCchan(int ccChannel){ m_CCchan = ccChannel; };
    
    int getValue(){ return m_value; };
    void setValue(int value);
    
    string getNoteInstrument(){ return m_instrument;};
    
    int getNoteId(){ return m_noteId;};
    
    void cleanNote(){ bNoteChanged = false; };
    
protected:
    int m_type;
    string m_note;
    int m_source;
    int m_CCchan;
    int m_value;
    int m_noteId;
    string m_instrument;
    int m_noteStatus;
    
    NoteParameters noteParams;
    
    bool bNoteChanged;
};

#endif /* defined(__FluidMotion__FluidNote__) */
