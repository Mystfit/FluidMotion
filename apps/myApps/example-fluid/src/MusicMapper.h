//
//  MusicMapper.h
//  FluidMotion
//
//  Created by Byron Mallett on 23/10/12.
//
//

#ifndef __FluidMotion__MusicMapper__
#define __FluidMotion__MusicMapper__

#include <iostream>
#include <vector>
#include "ofxXmlSettings.h"

using namespace std;

#define NOTE_MIDDLE_C 60;
enum notesNames{ NOTE_C = 0, NOTE_CSHARP, NOTE_D, NOTE_DSHARP, NOTE_E, NOTE_F, NOTE_FSHARP, NOTE_G, NOTE_GSHARP, NOTE_A, NOTE_ASHARP, NOTE_B };

struct ScaleDef {
    string name;
    vector<int> semitones;
};

static int paramValToNote(int paramValue);

class MusicMapper {
public:
    MusicMapper();
    
   private:
};

#endif /* defined(__FluidMotion__MusicMapper__) */
