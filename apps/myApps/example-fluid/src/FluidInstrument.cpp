//
//  FluidInstrument.cpp
//  FluidMotion
//
//  Created by Byron Mallett on 8/10/12.
//
//

#include "FluidInstrument.h"

FluidInstrument::FluidInstrument(){
    FluidInstrument("Instrument", "passthrough_out", 0, 0, INSTRUMENT_TYPE_MONOPHONIC, INSTRUMENT_PLAYS_CC);
}

FluidInstrument::FluidInstrument(string _name, string _device, int _channel, int _program, int _noteType, int _noteMapping)
{    
    name = _name;
    device = _device;
    channel = _channel;
    program = _program;
    noteType = _noteType;
    noteMapping = _noteMapping;
}







void FluidInstrument::createCC(int ccName, int ccValue, ofPoint coords, float area){
    FluidNote newNote(m_noteIdCounter, m_instrumentId, INSTRUMENT_PLAYS_CC);
    newNote.setParams(area, coords);
    newNote.setCCName(ccName);
    newNote.setCCValue(ccValue);
    
    addNote(newNote);
}

void FluidInstrument::createNote(string note, ofPoint coords, float area){
    FluidNote newNote(m_noteIdCounter, m_instrumentId, INSTRUMENT_PLAYS_NOTES);
    newNote.setParams(area, coords);
    newNote.setNote(note);

    addNote(newNote);
}


void FluidInstrument::createNoteFromBlob(ofxCvComplexBlob blob)
{
    FluidNote newNote;
    
    if(noteType == INSTRUMENT_PLAYS_CC){
        newNote = FluidNote(m_noteIdCounter++, m_instrumentId, INSTRUMENT_PLAYS_CC);
    }
    else if(noteType == INSTRUMENT_PLAYS_NOTES)
    {
        newNote = FluidNote(m_noteIdCounter++, m_instrumentId, INSTRUMENT_PLAYS_NOTES);
    }
        

}


void FluidInstrument::addNote(FluidNote note){
    activeNotes.push_back(note);
    ;
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


