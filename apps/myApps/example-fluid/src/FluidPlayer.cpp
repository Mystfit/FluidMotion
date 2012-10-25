//
//  FluidPlayer.cpp
//  FluidMotion
//
//  Created by Byron Mallett on 9/10/12.
//
//

#include "FluidPlayer.h"


FluidPlayer::FluidPlayer()
{
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
    loadScales();
    
    setRootNote(NOTE_C);
    setScale( getScaleByName("Minor"));
    setInstrument( getInstrumentByName("Digital Bells") );
}




/*
 * Start and stop performance
 */
void FluidPlayer::stopPerformance(){
    isPlaying = false;
    clockTick = 0;
    beatCount = 0;
    isBeat = 0;
    
}

void FluidPlayer::startPerformance(){
    isPlaying = true;
    clockTick = 0;
    beatCount = 0;
    isBeat = 0;
}


void FluidPlayer::setInstrument(FluidInstrument instrument)
{
    m_activeInstrument = instrument;
    midiOut.sendProgramChange(m_activeInstrument.channel, m_activeInstrument.program);
};



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
    ofDirectory instrumentDir("instruments");
    instrumentDir.listDir();
    vector<ofFile> instrumentFiles = instrumentDir.getFiles();
    
    for(int i=0; i < instrumentFiles.size(); i++)
    {
        ofxXmlSettings xmlInstrument;
        xmlInstrument.loadFile( instrumentDir.getOriginalDirectory() + instrumentFiles[i].getFileName() );
        xmlInstrument.pushTag("instrument");
        xmlInstrument.pushTag("properties");
        
        //Create a new instrument definition
        FluidInstrument instrument;
        instrument.setID(i);
        instrument.name = xmlInstrument.getValue("name", "");
        instrument.device = xmlInstrument.getValue("device", "");
        instrument.channel = xmlInstrument.getValue("channel", 0);
        instrument.program = xmlInstrument.getValue("program", 0);
        instrument.usesCCNoteTriggers = ofToBool(xmlInstrument.getValue("usesCCNoteTriggers", ""));
        
        
        int noteType, noteMapping;
        
        if(xmlInstrument.getValue("timbre", "") == "mono")
            instrument.noteType = INSTRUMENT_TYPE_MONOPHONIC;
        else if(xmlInstrument.getValue("timbre", "") == "poly")
            instrument.noteType = INSTRUMENT_TYPE_POLYPHONIC;
        
        if(xmlInstrument.getValue("noteMappings", "") == "note")
            instrument.noteMapping = INSTRUMENT_PLAYS_NOTES;
        else if(xmlInstrument.getValue("noteMappings", "") == "cc")
            instrument.noteMapping = INSTRUMENT_PLAYS_CC;
        
        xmlInstrument.popTag();
        xmlInstrument.pushTag("parameters");
        
        //Set instrument parameters and mappings -- CC
        int j;
        int numCCParams = xmlInstrument.getNumTags("cc");
        for(j = 0; j < numCCParams; j++)
        {
            xmlInstrument.pushTag("cc", j);
            InstrumentParameter param;
            param.noteType = INSTRUMENT_PLAYS_CC;
            param.channel = xmlInstrument.getValue("channel" , 0);
            param.source = instrument.getParamSourceFromString( xmlInstrument.getValue("source", "") );
            param.value = xmlInstrument.getValue("value" , 0);
            param.lowerNoteRange = xmlInstrument.getValue("min", 0);
            param.upperNoteRange = xmlInstrument.getValue("max", 0);
            instrument.addparam(param);
            xmlInstrument.popTag();
        }
        
        
        //Set instrument parameters and mappings -- NOTE
        int numNoteParams = xmlInstrument.getNumTags("note");        
        for(j = 0; j < numNoteParams; j++)
        {
            xmlInstrument.pushTag("note", j);
            InstrumentParameter param;
            param.noteType = INSTRUMENT_PLAYS_NOTES;
            param.source = instrument.getParamSourceFromString( xmlInstrument.getValue("source", "") );
            param.lowerNoteRange = xmlInstrument.getValue("min", 0);
            param.upperNoteRange = xmlInstrument.getValue("max", 0);
            instrument.addparam(param);
            xmlInstrument.popTag();
        }
        
        instrumentList.push_back(instrument);
    }
     
}



/*
 * Get scales by descriptive name
 */
ScaleDef FluidPlayer::getScaleByName(string name)
{
    for (int i = 0; i < scaleList.size(); i++){
        if (scaleList[i].name == name) return scaleList[i];
    }
}


/*
 * Load external scale definitions
 */
void FluidPlayer::loadScales()
{
    ofxXmlSettings scaleXml;
    scaleXml.loadFile( "scales.xml" );
    scaleXml.pushTag("scales");
    
    int numScales = scaleXml.getNumTags("scale");
    for(int i = 0; i < numScales; i++)
    {
        ScaleDef scale;
        scaleXml.pushTag("scale", i);
        scale.name = scaleXml.getValue("name", "");
        
        scaleXml.pushTag("semitones");
        int numSemitones = scaleXml.getNumTags("semitone");
        
        for(int j = 0; j < numSemitones; j++)
            scale.semitones.push_back(scaleXml.getValue("semitone", 0, j) );
        
        scaleList.push_back(scale);
        scaleXml.popTag();
        scaleXml.popTag();
    }
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
        
    } else if(clockTick % DEMISEMIQUAVER == 0){
        updateNotes();
        
    } else if(clockTick % baseNoteMessageLength) {
        ofLog(OF_LOG_VERBOSE, "Smallest note");
        isBeat = false;
        isBar = false;
    }
    
    clockPastTime = ofGetElapsedTimeMillis(); //int
    clockTick++; //int
}


vector<FluidNote> FluidPlayer::blobsToNotes(vector<BlobParam> blobParameters)
{
    _blobParams = blobParameters;
    vector<FluidNote> outputNotes;
    int i,j;
    
    for(i = 0; i < _blobParams.size(); i++)
    {
        //Create note
        if(_blobParams[i].isDirty)
        {
            outputNotes = m_activeInstrument.createNotesFromBlobParameters( _blobParams[i] );
            _blobParams[i].isDirty = false;
            
        //Check if notes need to be resent
        } else {
            //outputNotes = m_activeInstrument.createNotesFromBlobParameters( _blobParams[i] );
        }
    }
    
    
    //If notes exist already, update them, remove notes that don't match
    int activeNotesIndex;
    bool noteExists;
    for(i = 0; i < outputNotes.size(); i++)
    {
        activeNotesIndex = 0;
        noteExists = false;
        
        while( activeNotesIndex < m_activeInstrument.activeNotes.size() )
        {
            
            
            if(m_activeInstrument.activeNotes[i].getNoteId() == outputNotes[j].getNoteId() && m_activeInstrument.activeNotes[i].getSource() == outputNotes[j].getSource() )
            {
                if(m_activeInstrument.activeNotes[i].getValue() == outputNotes[j].getValue())
                {
                    m_activeInstrument.activeNotes[i].setClean();
                } else {
                    m_activeInstrument.activeNotes[i].setValue( outputNotes[j].getValue() );
                    m_activeInstrument.activeNotes[i].setDirty();
                }
                
                noteExists = true;
                break;
            }
            
            activeNotesIndex++;
        }
        
        if(!noteExists)
            m_activeInstrument.activeNotes.push_back(outputNotes[i]);
    }
    
    return outputNotes;
}


int FluidPlayer::getBlobIndexFromNoteId(int blobId)
{
    int index = -1;
    int blobIndex = 0;
    
    while(blobIndex < _blobParams.size())
    {
        if(blobId == _blobParams[blobIndex].id){
            index = blobIndex;
            break;
        }
        blobIndex++;
    }
    
    ofLog(OF_LOG_NOTICE, "Matching blob to note:" + ofToString(blobIndex));
    
    return index;
}





/*
 * Get instruments by name rather than id
 */
void FluidPlayer::updateNotes()
{
    //All of the following can only occur AFTER we've parsed blobs to notes!
    
    //Send notes via midi
    for(int i = 0; i < m_activeInstrument.activeNotes.size(); i++)
    {
        FluidNote currNote = m_activeInstrument.activeNotes[i];
        
        //Turn off notes that have no blobs assigned
//        if(getBlobIndexFromNoteId(currNote.getNoteId()) == -1)
//        {
//            if(m_activeInstrument.usesCCNoteTriggers){
//                InstrumentParameter noteOffParam = m_activeInstrument.getParamFromSource(INSTRUMENT_SOURCE_CCNOTEOFF);
//                
//                FluidNote ccNoteOffMessage(currNote.getNoteId(), m_activeInstrument.name, INSTRUMENT_PLAYS_CC);
//                ccNoteOffMessage.setValue(noteOffParam.value);
//                ccNoteOffMessage.setCCchan(noteOffParam.channel);
//                ccNoteOffMessage.setSource(noteOffParam.source);
//                ccNoteOffMessage.setDirty();
//                m_activeInstrument.activeNotes.push_back(ccNoteOffMessage);
//                //currNote.setStatus(OFF);
//                m_activeInstrument.isPlayingNote = false;
//            } else if(m_activeInstrument.noteMapping == INSTRUMENT_PLAYS_NOTES) {
//                
//                m_activeInstrument.activeNotes[i].setDirty();
//                m_activeInstrument.activeNotes[i].setStatus(OFF);
//                m_activeInstrument.isPlayingNote = false;
//            }
//        }
        
        
        //Play the note if it has changed
        if(currNote.getStatus() == ON)
        {
            if(currNote.isDirty()){
                if(currNote.getType() == INSTRUMENT_PLAYS_NOTES){
                    midiOut.sendNoteOn(m_activeInstrument.channel, currNote.getValue());
                }else if(currNote.getType() == INSTRUMENT_PLAYS_CC){
                    midiOut.sendControlChange(m_activeInstrument.channel, currNote.getCCchan(), currNote.getValue());
                    currNote.setStatus(OFF);
                }
                currNote.setClean();
            }
        }
        
        //Turn notes off and remove if flagged to remove
        if(currNote.getStatus() == OFF)
        {
            if(currNote.isDirty()){
                if(currNote.getType() == INSTRUMENT_PLAYS_NOTES){
                    midiOut.sendNoteOff(m_activeInstrument.channel, currNote.getValue());
                }else if(currNote.getType() == INSTRUMENT_PLAYS_CC && currNote.getSource() == INSTRUMENT_SOURCE_CCNOTEOFF){
                    midiOut.sendControlChange(m_activeInstrument.channel, currNote.getCCchan(), currNote.getValue());
                }
                currNote.setClean();
            }

            m_activeInstrument.removeNote(currNote);
        }
    }

    
    
}








