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

FluidInstrument::FluidInstrument(string _name, string _device, int _channel, int _program, int _timbreType, int _noteMapping)
{    
    name = _name;
    device = _device;
    channel = _channel;
    program = _program;
    _timbreType = _timbreType;
    noteMapping = _noteMapping;
}


vector<FluidNote> FluidInstrument::createNotesFromBlobParameters(BlobParam blobParameter)
{    
    vector<FluidNote> noteList;
    
    for(int i = 0; i < params.size(); i++)
    {
        int noteValue;
        float paramValue;

        if(params[i].noteType == INSTRUMENT_PLAYS_NOTES)
        {
            //Map the note from the source
            paramValue = blobParamValueFromSource(blobParameter, params[i].source );
            noteValue = lerpNote(paramValue, params[i].lowerNoteRange, params[i].upperNoteRange);
            
            FluidNote noteOnMessage(blobParameter.id, name, INSTRUMENT_PLAYS_NOTES);
            noteOnMessage.setValue(noteValue);
            noteOnMessage.setSource(params[i].source);
            noteOnMessage.setDirty();

            noteList.push_back(noteOnMessage);
        
        } else if(params[i].noteType == INSTRUMENT_PLAYS_CC)
        {
            
            if(usesCCNoteTriggers)
            {
                if(!isPlayingNote){
                    InstrumentParameter noteOnParam = getParamFromSource(INSTRUMENT_SOURCE_CCNOTEON);
                    
                    FluidNote ccNoteOnMessage(blobParameter.id, name, INSTRUMENT_PLAYS_CC);
                    ccNoteOnMessage.setValue(noteOnParam.value);
                    ccNoteOnMessage.setCCchan(noteOnParam.channel);
                    ccNoteOnMessage.setSource(noteOnParam.source);
                    ccNoteOnMessage.setDirty();
                    noteList.push_back(ccNoteOnMessage);
                    isPlayingNote = true;
                }
            }
            
            
            //Send cc messages, ignore ccNoteOns and ccNoteOffs
            if(params[i].source != INSTRUMENT_SOURCE_CCNOTEON && params[i].source != INSTRUMENT_SOURCE_CCNOTEOFF)
            {
                paramValue = blobParamValueFromSource(blobParameter, params[i].source );
                noteValue = lerpNote(paramValue, params[i].upperNoteRange, params[i].lowerNoteRange);
                
                FluidNote ccMessage(blobParameter.id, name, INSTRUMENT_PLAYS_CC);
                ccMessage.setCCchan(params[i].channel);
                ccMessage.setValue(noteValue);
                            
                ccMessage.setSource(params[i].source);
                ccMessage.setDirty();
                noteList.push_back(ccMessage);
            }
            
        }
            
    }

    return noteList;
}

vector<InstrumentParameter> FluidInstrument::getParametersByTagType(int paramType)
{
    vector<InstrumentParameter> paramList;
    for(int i = 0; i < params.size(); i++)
    {
        if(params[i].noteType == paramType) paramList.push_back(params[i]);
    }
    
    return paramList;
}


float FluidInstrument::blobParamValueFromSource(BlobParam blobParam, int source)
{
    switch(source){
        case INSTRUMENT_SOURCE_BLOBX: return blobParam.position.x; break;
        case INSTRUMENT_SOURCE_BLOBY: return blobParam.position.y; break;
        case INSTRUMENT_SOURCE_CURVATURE: return blobParam.curvature; break;
        case INSTRUMENT_SOURCE_AREA: return blobParam.area; break;
        case INSTRUMENT_SOURCE_INTENSITY: return blobParam.intensity; break;
    }

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



