#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <initializer_list>
using byte=uint8_t;
uint32_t millis();
#define TWO_PI 6.28318530717958647692
inline double radians(double d){return d*0.01745329251994329577;}
inline double degrees(double r){return r*57.2957795130823208768;}
inline double sq(double v){return v*v;}
