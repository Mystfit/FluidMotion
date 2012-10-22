//
//  FluidNote.cpp
//  FluidMotion
//
//  Created by Byron Mallett on 8/10/12.
//
//

#include "FluidNote.h"

FluidNote::FluidNote()
{
}

FluidNote::FluidNote(int noteId, int instrumentId, int type){
    m_type = type;
    m_noteId = noteId;
    m_noteStatus = ON;
    m_instrumentId = instrumentId;
};

void FluidNote::setNote(string note){
    m_note = note;
    bNoteChanged = true;
}


void FluidNote::setCCValue(int ccVal){
    m_ccValue = ccVal;
    bNoteChanged = true;
}

