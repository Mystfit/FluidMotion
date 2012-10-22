//
//  FluidPlayer.cpp
//  FluidMotion
//
//  Created by Byron Mallett on 9/10/12.
//
//

#include "FluidPlayer.h"


FluidPlayer::FluidPlayer(){
    
    //MIDI params
    midiOut.listPorts(); // via instance
    midiOut.openPort("passthrough_in");	// by name
    midiIn.openPort("passthrough_out");	// by name
    midiIn.ignoreTypes(false, false, false);
    midiIn.addListener(this);
    
    baseNoteMessageLength = DEMISEMIQUAVER;
    upperTimesig = 4;
    lowerTimesig = 4;
    
    loadInstruments();
}






/*
 * Start and stop performance
 */
void FluidPlayer::stopPerformance(){
    isPlaying = false;
    clockTick = 0;
    beatCount = 0;
    isBeat = 0;
    
    //DEBUG - Manual noteOff for Kaossilator
    m_activeInstrument.createCC(92, 0, ofPoint(64,64), 25);
}

void FluidPlayer::startPerformance(){
    isPlaying = true;
    clockTick = 0;
    beatCount = 0;
    isBeat = 0;
    
    //DEBUG - Manual noteOn for Kaossilator
    m_activeInstrument.createCC(92, 127, ofPoint(64,64), 25);
}


/*
 * Get instruments by name rather than id
 */
FluidInstrument FluidPlayer::getInstrumentByName(string name){
    for (int i = 0; i < instrumentList.size(); i++){
        if (instrumentList[i].name == name) return instrumentList[i];
    }
}


/*
 * Load external instrument definitions
 */
void FluidPlayer::loadInstruments()
{
    //DEBUG - Test xml stuff
    ofDirectory instrumentDir("instruments");
    instrumentDir.listDir();
    
    vector<ofFile> instrumentFiles = instrumentDir.getFiles();
    
    for(int i=0; i < instrumentFiles.size(); i++){
        ofxXmlSettings xmlInstrument;
        xmlInstrument.loadFile( instrumentDir.getOriginalDirectory() + instrumentFiles[i].getFileName() );
        
        FluidInstrument instrument;
        instrument.setID(i);
        instrument.name = xmlInstrument.getValue("instrument:properties:name", "");
        instrument.device = xmlInstrument.getValue("instrument:properties:device", "");
        instrument.channel = xmlInstrument.getValue("instrument:properties:channel", 0);
        instrument.program = xmlInstrument.getValue("instrument:properties:program", 0);
        
        int noteType, noteMapping;
        
        if(xmlInstrument.getValue("instrument:properties:timbre", "") == "mono")
            instrument.noteType = INSTRUMENT_TYPE_MONOPHONIC;
        else if(xmlInstrument.getValue("instrument:properties:timbre", "") == "poly")
            instrument.noteType = INSTRUMENT_TYPE_POLYPHONIC;
        
        if(xmlInstrument.getValue("instrument:properties:noteMappings", "") == "note")
            instrument.noteMapping = INSTRUMENT_PLAYS_NOTES;
        else if(xmlInstrument.getValue("instrument:properties:noteMappings", "") == "cc")
            instrument.noteMapping = INSTRUMENT_PLAYS_CC;
        
        instrumentList.push_back(instrument);
    }
    
    //Set instrument to default upon loading
    if(instrumentList.size() > 0)  setInstrument(getInstrumentByName("Default Instrument"));
}





/**
 *
 * External midi message listener
 *
 */
void FluidPlayer::newMidiMessage(ofxMidiMessage & eventArgs) {
    
    lastMessage = eventArgs;
    
    switch (eventArgs.status) {
        case MIDI_START:
            ofLog(OF_LOG_NOTICE, eventArgs.toString());
            startPerformance();
            break;
        case MIDI_STOP:
            ofLog(OF_LOG_NOTICE, eventArgs.toString());
            stopPerformance();
        case MIDI_SONG_POS_POINTER:
            ofLog(OF_LOG_NOTICE, eventArgs.toString());
            break;
        case MIDI_TIME_CLOCK:
            if(isPlaying) musicTick();
            break;
        default:
            break;
    }
    
   
}


/*
 * Midi clock tick triggered from external clock
 */
void FluidPlayer::musicTick()
{
    // If we use an external device to set the bpm, then we get around
    // Openframeworks updating at a low fps screwing with the clock
    
    //One crotchet per 24 ticks
    if(clockTick % CROTCHET == 0)
    {
        isBeat = true;
        beatDirty = true;       //Set the beat as dirty so OF updates it when it has time
        
        //Get the bpm from the last 4 notes
        if( beatCount >= lowerTimesig)
            beatCount = 0;
        
        if(beatCount == 0){
            isBar = true;
            firstBeatMs = ofGetElapsedTimeMillis();
            ofLog(OF_LOG_NOTICE, "NEW BAR");
        } else {
            ofLog(OF_LOG_NOTICE, "BEAT");
        }
        
        if(ofGetElapsedTimeMillis() - firstBeatMs != 0)
            bpm = 60000 * beatCount / (ofGetElapsedTimeMillis() - firstBeatMs);
        
        if(clockPeriodValue >= upperTimesig-1)
        {
            clockPeriodValue = 0;
            clockTick = 0;
        } else {
            clockPeriodValue++;
        }
        
        beatCount++;
        
    } else if(clockTick % baseNoteMessageLength) {
        ofLog(OF_LOG_VERBOSE, "Smallest note");
        isBeat = false;
        isBar = false;
    }
    
    clockPastTime = ofGetElapsedTimeMillis(); //int
    clockTick++; //int
}



/*
 * Get instruments by name rather than id
 */
void FluidPlayer::updateNotes(vector<ofxCvComplexBlob> blobs)
{
    int i;
    int noteIndex;
    
    //Create and delete notes based on blob detection outcome

    if(blobs.size() > m_activeInstrument.activeNotes.size() )  //Create new notes
    {
        for(i = 0; i < blobs.size(); i++)
        {
            noteIndex = getNoteIndexFromBlob(blobs[i]);
            
            if(noteIndex >= 0)
                m_activeInstrument.activeNotes[noteIndex].setParams(blobs[i].getArea(), blobs[i].getBoundingBox().getCenter() );
            else
                m_activeInstrument.createCC(92, CC_MAX_VALUE, blobs[i].getBoundingBox().getCenter(), blobs[i].getArea() );
            
        }
        
    }
    
    else if( blobs.size() < m_activeInstrument.activeNotes.size() )     //Flag expired notes
    {
        for(i = 0; i < m_activeInstrument.activeNotes.size(); i++){
            int blobIndex = 0;
            while( blobIndex < blobs.size() )
            {
                noteIndex = getNoteIndexFromBlob(blobs[blobIndex]);
                
                if(noteIndex < 0)
                    m_activeInstrument.activeNotes[i].setStatus(OFF);

                blobIndex++;
            }
        }
    }
    
    
    //Send notes via midi
    for(int i = 0; i < m_activeInstrument.activeNotes.size(); i++)
    {
        FluidNote currNote = m_activeInstrument.activeNotes[i];
        
        if(currNote.getStatus() == ON){
            if(currNote.getNoteType() == INSTRUMENT_PLAYS_NOTES){
                
            } else if(currNote.getNoteType() == INSTRUMENT_PLAYS_CC){
                ofLog(OF_LOG_VERBOSE, "NOTE ON");
                midiOut.sendControlChange(m_activeInstrument.channel, currNote.getCCName(), currNote.getCCValue() );
            }
            
            currNote.setStatus(HOLD);        //KAOSSILATOR override - Only play the first note.
        }
        
        if(currNote.getStatus() == HOLD){
            midiOut.sendControlChange(m_activeInstrument.channel, 12, int(currNote.getParams().fluidPosition.x/256 * 127) );
            midiOut.sendControlChange(m_activeInstrument.channel, 13, int(currNote.getParams().fluidPosition.y/256 * 127) );
        }
        
        if(currNote.getStatus() == OFF){
            if(currNote.getNoteType() == INSTRUMENT_PLAYS_NOTES){
                //Send midi noteOff
            } else if(currNote.getNoteType() == INSTRUMENT_PLAYS_CC){
                ofLog(OF_LOG_VERBOSE, "NOTE OFF");
                midiOut.sendControlChange(m_activeInstrument.channel, 92, 0);
            }
            
            m_activeInstrument.removeNote(currNote);
        }
    }

    
    
}







/*
 * Match active notes against current list of blobs
 */
int FluidPlayer::getNoteIndexFromBlob(ofxCvComplexBlob blob)
{
    int i;
    int noteIndex = -1;
    float smallestDist = 0.0f;
    float distThreshold = 5.0f;
    
    for(i = 0; i < m_activeInstrument.activeNotes.size(); i++)
    {
        float dist = blob.getBoundingBox().getCenter().distance( m_activeInstrument.activeNotes[i].getParams().fluidPosition );
        
        if(smallestDist == 0.0f) smallestDist = dist;
        
        if( dist < distThreshold && dist <= smallestDist){
            noteIndex = i;
            smallestDist = dist;
        }
    }
    
    return noteIndex;
}





