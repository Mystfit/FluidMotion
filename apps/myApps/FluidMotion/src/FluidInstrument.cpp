//
//  FluidInstrument.cpp
//  FluidMotion
//
//  Created by Byron Mallett on 8/10/12.
//
//

#include "FluidInstrument.h"

FluidInstrument::FluidInstrument(){
    m_noteIdCounter = 0;
}

void FluidInstrument::createCC(int ccName, int ccValue, ofPoint coords, float area){
    FluidNote newNote(m_noteIdCounter, m_instrumentId, CC);
    newNote.setParams(area, coords);
    newNote.setCCName(ccName);
    newNote.setCCValue(ccValue);
    
    addNote(newNote);
}

void FluidInstrument::createNote(string note, ofPoint coords, float area){
    FluidNote newNote(m_noteIdCounter, m_instrumentId, NOTE);
    newNote.setParams(area, coords);
    newNote.setNote(note);

    addNote(newNote);
}

void FluidInstrument::addNote(FluidNote note){
    activeNotes.push_back(note);
    m_noteIdCounter++;
}

void FluidInstrument::removeNote(FluidNote note){
    int i;
    
    for(i = 0; i < activeNotes.size(); i++)
    {
        if(activeNotes[i].getNoteId() == note.getNoteId()){
            activeNotes.erase(activeNotes.begin() + i);
            return;
        }
    }
}


