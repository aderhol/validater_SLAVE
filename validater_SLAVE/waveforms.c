#include <stdint.h>
#include <math.h>

#include <waveforms.h>

double sine(int64_t t, double fi, double T, double A, double off)
{
    t = fmod(t, T);
    double tM = t - (fi / 360) * T;
    double val;
    val = sin(tM * ((2 * M_PI) / T)) * fabs(A);
    return val + off;
}

double triangle(int64_t t, double fi, double T, double A, double off)
{
    t = fmod(t, T);
    double tM = t - (fi / 360) * T;
    double val;
    if(tM < 0)
        tM += T;
    if(tM > ((3 * T) / 4))
        tM -= T;
    if(tM <= (T / 4))
        val = ((2 * fabs(A)) / (T / 2)) * tM;
    else
        val = -((2 * fabs(A)) / (T / 2)) * (tM - (T / 2));
    return val + off;
}

double sawtooth(int64_t t, double fi, double T, double min, double max)
{
    t = fmod(t, T);
    double tM = t - (fi / 360) * T;
    double val;
    if(tM < 0)
        tM += T;
    val = ((max - min) / T) * tM + min;
    return val;
}

double square(int64_t t, double fi, double T, double D, double A, double off)
{
    t = fmod(t, T);
    double tM = t - (fi / 360) * T;
    double val;
    if(tM >= 0)
        val = (tM < (T * (D / 100))) ? fabs(A) : -fabs(A);
    else
        val = (fabs(tM) < (T * (D / 100)) - T) ? fabs(A) : -fabs(A);
    return val + off;
}
