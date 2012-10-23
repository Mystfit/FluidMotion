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
    
    activeNotes.push_back(newNote);
}

void FluidInstrument::createNote(string note, ofPoint coords, float area){
    FluidNote newNote(m_noteIdCounter, m_instrumentId, INSTRUMENT_PLAYS_NOTES);
    newNote.setParams(area, coords);
    newNote.setNote(note);

    activeNotes.push_back(newNote);
}


void FluidInstrument::createNoteFromBlob(ofxCvComplexBlob blob)
{
    FluidNote newNote(m_noteIdCounter++, m_instrumentId, noteType);

    if(noteType == INSTRUMENT_PLAYS_CC){
        //Find the cc channel that is mapped to the blobX property
        
    }
    else if(noteType == INSTRUMENT_PLAYS_NOTES)
    {
        //Need to sort out mapping a note from a coordinate to a letter value in here
        
        //newNote.setNote( mappedNote )
    }
    
    

    
    activeNotes.push_back(newNote);
}


void FluidInstrument::removeNote(FluidNote note){    
    for(int i = 0; i < activeNotes.size(); i++)
    {
        if(activeNotes[i].getNoteId() == note.getNoteId()){
            activeNotes.erase(activeNotes.begin() + i);
            return;
        }
    }
}


InstrumentParameter FluidInstrument::getParamFromSource(int source)
{
    for(int i = 0; i < params.size(); i++)
        if(params[i].source == source )
            return params[i]; 
}



/*
 * Converts string parameter tags to int constants
 */
int FluidInstrument::getParamSourceFromString(string source)
{
    if(source == "blobX")
        return INSTRUMENT_SOURCE_BLOBX;
    else if(source == "blobY")
        return INSTRUMENT_SOURCE_BLOBY;
    else if(source == "curvature")
        return INSTRUMENT_SOURCE_CURVATURE;
    else if(source == "area")
        return INSTRUMENT_SOURCE_AREA;
    else if(source == "intensity")
        return INSTRUMENT_SOURCE_INTENSITY;
    else if(source == "noteOn")
        return INSTRUMENT_SOURCE_CCNOTEON;
    else if(source == "noteOff")
        return INSTRUMENT_SOURCE_CCNOTEOFF;
}



