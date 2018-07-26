#ifndef WAVEFORMS_H
#define WAVEFORMS_H

#include <stdint.h>

double sine(int64_t t, double fi, double T, double A, double off);
double triangle(int64_t t, double fi, double T, double A, double off);
double sawtooth(int64_t t, double fi, double T, double min, double max);
double square(int64_t t, double fi, double T, double D, double A, double off);

#endif
