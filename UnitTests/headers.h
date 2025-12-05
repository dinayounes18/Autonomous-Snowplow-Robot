#ifndef HEADERS_H
#define HEADERS_H

#define MAX 255
#define MIN 0
#define DEG_90 2050
#define DEG_180 DEG_90 * 2
#define DEG_135 DEG_90 * 1.5
#define DEG_45 DEG_90 / 2
#define LEFT false
#define RIGHT true
#define FWD false
#define REV true
#define DEBUG true
#define SAMPLES 1024
#define THRESHOLD 175
#define TRIG1 US_PINS1[0] 
#define ECHO1 US_PINS1[1]
#define TRIG2 US_PINS2[0]
#define ECHO2 US_PINS2[1]
#define SOUND 0.034
#define DISTANCE_THRESHOLD 15

const int SPE_PINS [] = {8, 9, 10, 11}; // M1, M2, M3, M4
const int DIR_PINS [] = {7, 6, 5, 4}; // M1, M2, M3, M4
const int LINE_PINS_FWD [] = {A9, A10, A11}; // OUT1, OUT2, OUT3
const int LINE_PINS_LEFT [] = {A0, A1, A2}; // OUT1, OUT2, OUT3
const int US_PINS1 [] = {14, 15}; // TRIG1, ECHO1
const int US_PINS2 [] = {16, 17}; // TRIG2, ECHO2

void initPins();


#endif