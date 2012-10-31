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
    currentInstrumentIndex = 0;
    
    loadInstruments();
    loadScales();
    
    setRootNote(NOTE_C);
    setScale( getScaleByName("Minor"));
    
    setInstrument( instrumentList[0] );
}




/*
 * Start and stop performance
 */
void FluidPlayer::stopPerformance(){
    isPlaying = false;
    clockTick = 0;
    beatCount = 0;
    isBeat = 0;
    midiOut.sendControlChange(2, 81, 127);
}

void FluidPlayer::startPerformance(){
    isPlaying = true;
    clockTick = 0;
    beatCount = 0;
    isBeat = 0;
    midiOut.sendControlChange(2, 80, 127);
}


void FluidPlayer::setInstrument(FluidInstrument instrument)
{
    midiOut.sendControlChange(m_activeInstrument.channel, MIDI_ALL_NOTES_OFF, 0);

    m_activeInstrument = instrument;
    //midiOut.sendProgramChange(m_activeInstrument.channel, m_activeInstrument.program);
}

void FluidPlayer::nextInstrument()
{
    int instrumentIndex = getInstrumentIndex(m_activeInstrument);
    if(instrumentIndex == -1)
        return;
    
    if( instrumentIndex+1 >= instrumentList.size())
        instrumentIndex = 0;
    else
        instrumentIndex++;
    setInstrument(instrumentList[instrumentIndex]);
}


void FluidPlayer::prevInstrument()
{
    int instrumentIndex = getInstrumentIndex(m_activeInstrument);
    if(instrumentIndex == -1)
        return;

    if( instrumentIndex-1 < 0 )
        instrumentIndex = instrumentList.size() - 1;
    else
        instrumentIndex--;

    setInstrument(instrumentList[instrumentIndex]);
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
 * Get instruments by name rather than id
 */
int FluidPlayer::getInstrumentIndex(FluidInstrument instrument){
    int index = -1;
    for (int i = 0; i < instrumentList.size(); i++){
        if (instrumentList[i].name == instrument.name){
            index = i;
        }
    }
    return index;
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
        
        instrument.dyeColour = ofVec3f(xmlInstrument.getValue("colourR",1.0f), xmlInstrument.getValue("colourG",1.0f), xmlInstrument.getValue("colourB",1.0f));
        instrument.usesCCNoteTriggers = ofToBool(xmlInstrument.getValue("usesCCNoteTriggers", ""));
        
        int timbreType, noteMapping;
        
        if(xmlInstrument.getValue("timbre", "") == "mono")
            instrument.timbreType = INSTRUMENT_TYPE_MONOPHONIC;
        else if(xmlInstrument.getValue("timbre", "") == "poly")
            instrument.timbreType = INSTRUMENT_TYPE_POLYPHONIC;
        
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
            instrument.addNoteParam(param, true);
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
            instrument.addNoteParam(param, false);

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
        sendNotes();
        
    } else if(clockTick % baseNoteMessageLength) {
        ofLog(OF_LOG_VERBOSE, "Smallest note");
        isBeat = false;
        isBar = false;
        m_activeInstrument.tickNoteParams();
        sendCC();
    }
    
    clockPastTime = ofGetElapsedTimeMillis(); //int
    clockTick++; //int
}


void FluidPlayer::blobsToNotes(vector<BlobParam> blobParameters)
{
    _blobParams = blobParameters;
    vector<FluidNote> outputNotes;
    int i,j;
    
    if(blobParameters.size() == 0)
        return;
    
    vector<int> newParamValues(m_activeInstrument.params.size(), 0);
    
    for(i = 0; i < _blobParams.size(); i++)
    {
        vector<int> blobValues = m_activeInstrument.createNotesFromBlobParameters( _blobParams[i] );
       
        for(j=0; j < blobValues.size(); j++){
            if(m_activeInstrument.noteParams[i].getType() == INSTRUMENT_PLAYS_NOTES)
                newParamValues[j] = newParamValues[j];
            else if(m_activeInstrument.noteParams[i].getType() == INSTRUMENT_PLAYS_CC){
                newParamValues[j] = newParamValues[j] + blobValues[j];
            }

        }
        _blobParams[i].isDirty = false;
    }
    
    if(_blobParams.size() < 1 && bClearAllNotes != true)
        bClearAllNotes = true;
    
    for(i=0; i < m_activeInstrument.noteParams.size(); i++){
        if(m_activeInstrument.noteParams[i].getType() == INSTRUMENT_PLAYS_NOTES)
            m_activeInstrument.noteParams[i].setPreferredValue(newParamValues[i]);
        else if(m_activeInstrument.noteParams[i].getType() == INSTRUMENT_PLAYS_CC)
            m_activeInstrument.noteParams[i].setPreferredValue(newParamValues[i] / newParamValues.size());
        m_activeInstrument.noteParams[i].setDirty();
    }
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
    
    if(blobIndex == -1);
        ofLog(OF_LOG_NOTICE, "Blob id:" + ofToString(blobId) + " is expired");
    
    return index;
}



/*
 *Send notes to devices and clean up
 */

void FluidPlayer::sendNotes()
{
    
    if(bClearAllNotes){
        midiOut.sendControlChange(m_activeInstrument.channel, MIDI_ALL_NOTES_OFF, 0);
    }
    
    
    for(int i = 0; i < m_activeInstrument.noteParams.size(); i++)
    {
        
        //Turn notes off and remove if flagged to remove
        if(m_activeInstrument.noteParams[i].getStatus() == OFF)
        {
            if(m_activeInstrument.noteParams[i].getType() == INSTRUMENT_PLAYS_NOTES){
                if(bClearAllNotes) m_activeInstrument.noteParams[i].setClean();

                if(m_activeInstrument.noteParams[i].isDirty()){
                    midiOut.sendNoteOff(m_activeInstrument.channel, m_activeInstrument.noteParams[i].getValue(), 64);
                    m_activeInstrument.noteParams[i].setLastNote(m_activeInstrument.noteParams[i].getValue());
                    m_activeInstrument.noteParams[i].setClean();
                }
                
            }
        }
        
        //Play the note if it has changed
        if(m_activeInstrument.noteParams[i].getStatus() == ON)
        {
            if(m_activeInstrument.noteParams[i].getType() == INSTRUMENT_PLAYS_NOTES){
                if(m_activeInstrument.noteParams[i].isDirty()){
            
                    if(m_activeInstrument.noteParams[i].getValue() != m_activeInstrument.noteParams[i].getLastNote())
                    {
                        midiOut.sendNoteOn(m_activeInstrument.channel, m_activeInstrument.noteParams[i].getValue(),100);
                        midiOut.sendNoteOff(m_activeInstrument.channel, m_activeInstrument.noteParams[i].getLastNote(), 64);
                        m_activeInstrument.noteParams[i].setLastNote(m_activeInstrument.noteParams[i].getValue());
                    } else {
                        //m_activeInstrument.noteParams[i].setClean();
                    }
                }
                
                if(bClearAllNotes) m_activeInstrument.noteParams[i].setStatus(OFF);
            }
        }
    
    }
    
    if(bClearAllNotes) bClearAllNotes = false;

}



/*
 *Send CC to devices and clean up
 */

void FluidPlayer::sendCC()
{
    for(int i = 0; i < m_activeInstrument.noteParams.size(); i++)
    {
        
        //Play the note if it has changed
        if(m_activeInstrument.noteParams[i].getStatus() == ON)
        {
            if(m_activeInstrument.noteParams[i].getType() == INSTRUMENT_PLAYS_CC){

                if(m_activeInstrument.noteParams[i].isDirty()){
                    midiOut.sendControlChange(m_activeInstrument.channel, m_activeInstrument.noteParams[i].getCCchan(), m_activeInstrument.noteParams[i].getValue());
                    m_activeInstrument.noteParams[i].setClean();
                }
            }
        }
        
        //Turn notes off and remove if flagged to remove
        if(m_activeInstrument.noteParams[i].getStatus() == OFF)
        {
            if(m_activeInstrument.noteParams[i].getType() == INSTRUMENT_PLAYS_CC){
                if(m_activeInstrument.noteParams[i].getSource() == INSTRUMENT_SOURCE_CCNOTEOFF || m_activeInstrument.noteParams[i].getSource() == INSTRUMENT_SOURCE_CCNOTEON){
                    midiOut.sendControlChange(m_activeInstrument.channel, m_activeInstrument.noteParams[i].getCCchan(), m_activeInstrument.noteParams[i].getValue());
                    m_activeInstrument.noteParams[i].setClean();
                }
            }
        }
    }
}








