#pragma once

#include "../../wasm4.h"

// bowlingalley

// Area data
#define bowlingalleyAreaWidth 10
#define bowlingalleyAreaHeight 17
extern const uint8_t bowlingalley_area_data[85];

// Object data
#define pin0Coord 54, 8
#define pin0Type  OBJ_TYPE_PIN
#define pin1Coord 66, 8
#define pin1Type  OBJ_TYPE_PIN
#define pin2Coord 78, 8
#define pin2Type  OBJ_TYPE_PIN
#define pin3Coord 90, 8
#define pin3Type  OBJ_TYPE_PIN
#define pin4Coord 60, 16
#define pin4Type  OBJ_TYPE_PIN
#define pin5Coord 72, 16
#define pin5Type  OBJ_TYPE_PIN
#define pin6Coord 84, 16
#define pin6Type  OBJ_TYPE_PIN
#define pin8Coord 78, 24
#define pin8Type  OBJ_TYPE_PIN
#define pin7Coord 66, 24
#define pin7Type  OBJ_TYPE_PIN
#define pin9Coord 72, 32
#define pin9Type  OBJ_TYPE_PIN
#define ballCoord 76, 168
#define ballType  OBJ_TYPE_BALL
#define personCoord 72, 232
#define personType  OBJ_TYPE_PERSON

#define bowlingalleyObjCount 12
