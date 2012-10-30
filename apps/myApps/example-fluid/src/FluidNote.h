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
    
    int getSource(){return m_source;};
    void setSource(int source){m_source = source;};
      
    int getCCchan(){ return m_CCchan; };
    void setCCchan(int ccChannel){ m_CCchan = ccChannel; };
    
    int getValue(){ return m_value; };
    void setValue(int value);
    
    void setPreferredValue(int value){ m_preferredValue = value; };
    int getPreferredValue(){ return m_preferredValue; };
    
    string getNoteInstrument(){ return m_instrument;};
    
    int getNoteId(){ return m_noteId;};
    void setNoteId(int value){ m_noteId = value;};
    
    void setLastNote(int value){m_lastNote = value;};
    int getLastNote(){return m_lastNote;};

    
    void setDirty(){ bNoteChanged = true; };
    void setClean(){ bNoteChanged = false; };
    bool isDirty(){ return bNoteChanged;};
    bool isDampable;
    
protected:
    int m_type;
    string m_note;
    int m_source;
    int m_CCchan;
    int m_value;
    int m_preferredValue;
    int m_noteId;
    string m_instrument;
    int m_noteStatus;
    int m_lastNote;
    
    NoteParameters noteParams;
    
    bool bNoteChanged;
};


class FluidNoteGroup {
public:
    FluidNoteGroup();
    ~FluidNoteGroup();
    FluidNoteGroup(vector<FluidNote> noteArray);
    void addNote(FluidNote note);
    
private:
    vector<FluidNote> m_notes;
};

#endif /* defined(__FluidMotion__FluidNote__) */
