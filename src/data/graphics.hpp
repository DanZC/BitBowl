#ifndef DATA_GRAPHICS_HPP
#define DATA_GRAPHICS_HPP

#include "../wasm4.h"

// ball
#define ballWidth 64
#define ballHeight 64
#define ballFlags BLIT_2BPP
extern const uint8_t ball[1024];
// ball2
#define ball2Width 8
#define ball2Height 128
#define ball2Flags BLIT_2BPP
extern const uint8_t ball2[256];
// bowlingalley
#define bowlingalleyWidth 8
#define bowlingalleyHeight 88
#define bowlingalleyFlags BLIT_2BPP
extern const uint8_t bowlingalley[176];
// frame
#define frameWidth 8
#define frameHeight 32
#define frameFlags BLIT_1BPP
extern const uint8_t frame[32];
// meter
#define meterWidth 16
#define meterHeight 32
#define meterFlags BLIT_2BPP
extern const uint8_t meter[128];
// person
#define personWidth 16
#define personHeight 144
#define personFlags BLIT_2BPP
extern const uint8_t person[576];
// pins
#define pinsWidth 16
#define pinsHeight 80
#define pinsFlags BLIT_2BPP
extern const uint8_t pins[320];
// title
#define titleWidth 80
#define titleHeight 24
#define titleFlags BLIT_2BPP
extern const uint8_t title[480];

#endif //DATA_GRAPHICS_HPP