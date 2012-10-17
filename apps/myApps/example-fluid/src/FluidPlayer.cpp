//
//  FluidPlayer.cpp
//  FluidMotion
//
//  Created by Byron Mallett on 9/10/12.
//
//

#include "FluidPlayer.h"


FluidPlayer::FluidPlayer(){
    m_activeInstrument.setID(0);
    
    //MIDI params
    midiOut.listPorts(); // via instance
    midiOut.openPort("passthrough_in");	// by name
    
    channel = 1;
    synthChan = 2;
    effectsChan = 3;
	currentPgm = 0;
	note = 0;
	velocity = 0;
	pan = 0;
	bend = 0;
	touch = 0;
	polytouch = 0;
    
    m_activeInstrument.createCC(92, 127, ofPoint(64,64), 25);
}

void FluidPlayer::updateNotes(vector<ofxCvComplexBlob> blobs)
{
    int i;
    int noteIndex;
    
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
                else
                    m_activeInstrument.activeNotes[noteIndex].setParams(blobs[blobIndex].getArea(), blobs[blobIndex].getBoundingBox().getCenter() );
                
                blobIndex++;
            }
        }
    }
}


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

void FluidPlayer::update()
{
    int i;
    
    ofLog(OF_LOG_NOTICE, "NUM NOTES:"+ ofToString(m_activeInstrument.activeNotes.size()));
    
    for( i = 0; i < m_activeInstrument.activeNotes.size(); i++)
    {
        FluidNote currNote = m_activeInstrument.activeNotes[i];
        
        if(currNote.getStatus() == ON){
            if(currNote.getNoteType() == NOTE){
                               
            } else if(currNote.getNoteType() == CC){
                ofLog(OF_LOG_NOTICE, "NOTE ON");
                //DEBUG -- Hardcoded outputs to kaossilators!
                midiOut.sendControlChange(synthChan, currNote.getCCName(), currNote.getCCValue() );
                midiOut.sendControlChange(effectsChan, currNote.getCCName(), currNote.getCCValue() );
                //END DEBUG
            }
            
            //if(i ==0 )
            currNote.setStatus(HOLD);        //KAOSSILATOR override - Only play the first note.
        }
        
        if(currNote.getStatus() == HOLD){
            // ofLog(OF_LOG_NOTICE, "NOTE VAL");

            //DEBUG -- Hardcoded outputs to kaossilators!
            midiOut.sendControlChange(synthChan, 12, int(currNote.getParams().fluidPosition.x/256 * 127) );
            midiOut.sendControlChange(synthChan, 13, int(currNote.getParams().fluidPosition.y/256 * 127) );
            midiOut.sendControlChange(effectsChan, 12, int(currNote.getParams().fluidPosition.x/256 * 127) );
            midiOut.sendControlChange(effectsChan, 13, int(currNote.getParams().fluidPosition.y/256 * 127) );
            
            //END DEBUG
        }
        
        if(currNote.getStatus() == OFF){
            if(currNote.getNoteType() == NOTE){
                //Send midi noteOff
            } else if(currNote.getNoteType() == CC){
                ofLog(OF_LOG_NOTICE, "NOTE OFF");
                //DEBUG -- Hardcoded outputs to kaossilators!
                midiOut.sendControlChange(synthChan, 92, 0);
                midiOut.sendControlChange(effectsChan, 92, 0);
                //END DEBUG
            }
            
            m_activeInstrument.removeNote(currNote);
        }
    }
}
