#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "gen.h"

#include "waveforms.h"

#define root_3 (1.732050808)
double offSim(int64_t time)
{
    return root_3*sine(time, 0, 60*19, 2, 3);
}

double invRelErr(double x, double relErr) //inverse function of the relative error function
{
    return relErr * fabs(x) + x;
}

double genHDOP(int64_t time)
{
    double x = square(time, 0, 3600*1, 50, 0.3, 0.5);
    double err = triangle(time, 0, 60*13, 0.3, 0.1); //T:13m, +-30%, 10%
    return invRelErr(x, err);
}
double genVDOP(int64_t time)
{
    double x = triangle(time, 0, 3600*1, 0.3, 0.5);
    double err = triangle(time, 0, 60*23, 0.25, -0.15); //T:23m, +-25%, -15%
    return invRelErr(x, err);
}

double genPDOP(int64_t time)
{
    double x = sine(time, 0, 3600*1, 0.3, 0.5);
    double err = triangle(time, 0, 60*8, 0.37, 0); //T:8m, +-37%, 0%
    return invRelErr(x, err);
}

int genNumberOfSatellitesUsed(int64_t time)
{
    int PRN[12];
    return genUsedGPSSatelliteList(time, PRN) + genUsedGLONASSSatelliteList(time, PRN);
}

int genGPSSatellitesInView(int64_t time, Satellite* satellites)
{
    int i;
    for(i = 0; i < 10; i++){
        satellites[i].PRN = 10 + i;
        satellites[i].elevation = 50 + i;
        satellites[i].azimuth = 100 + 10 * i + i;
        satellites[i].used = (i < 5);
    }
    
    int maxSNR = satellites[0].SNR = sine(time, 175, 60*7, 5, 66) + sine(time, 0, 60*27, 7, 3);
    double avrSNR = sine(time, 0, 60*32, 10, 40);
    int sum = (int)(10 * avrSNR - maxSNR + 0.5) - 1*9;
    i = 1;
    while(sum > (maxSNR - 1))
        sum -= (satellites[i++].SNR = maxSNR - 1) - 1;
    satellites[i++].SNR = sum + 1;
    while(i < 10)
        satellites[i++].SNR = 1;    
    return 10;    
}

int genGLONASSSatellitesInView(int64_t time, Satellite* satellites)
{
    int i;
    for(i = 0; i < 10; i++){
        satellites[i].PRN = 70 + i;
        satellites[i].elevation = 30 + i;
        satellites[i].azimuth = 200 + 10 * i + i;
        satellites[i].used = (i < 3);
    }
    
    int maxSNR = satellites[0].SNR = sawtooth(time, 0, 60*14, 62, 75) + square(time, 0, 60*16, 78, 3, -2);
    double avrSNR = sine(time, 90, 60*32, 15, 35);
    int sum = (int)(10 * avrSNR - maxSNR + 0.5) - 1*9;
    i = 1;
    while(sum > (maxSNR - 1))
        sum -= (satellites[i++].SNR = maxSNR - 1) - 1;
    satellites[i++].SNR = sum + 1;
    while(i < 10)
        satellites[i++].SNR = 1;    
    return 10;    
}

int genUsedGPSSatelliteList(int64_t time, int* PRN)
{
    Satellite satellites[36];
    int usedCount = 0, viewCount = genGPSSatellitesInView(time, satellites);
    int i;
    for(i = 0; i < viewCount; i++)
        if(true == satellites[i].used)
            PRN[usedCount++] = satellites[i].PRN;
    return usedCount;
}

int genUsedGLONASSSatelliteList(int64_t time, int* PRN)
{
    Satellite satellites[36];
    int usedCount = 0, viewCount = genGLONASSSatellitesInView(time, satellites);
    int i;
    for(i = 0; i < viewCount; i++)
        if(true == satellites[i].used)
            PRN[usedCount++] = satellites[i].PRN;
    return usedCount;
}

double genAltitude(int64_t time)
{
    return sawtooth(time, 0, 60 * 3, 100, 1500) + offSim(time);
}

double genUndulation(int64_t time)
{
    return -16.57;
}

double genTrack(int64_t time)
{
    return 147.369;
}

double genSpeed(int64_t time)
{
    return 0;
}

Latitude genLatitude(int64_t time)
{
    double degree = sine(time, 0, 60 * 5, 5, 40) + offSim(time)*0.000008988;    
    Latitude ret;
    ret.degree = (int)degree;
    double a;
    ret.minute = 60 * modf(degree, &a);
    ret.direction = North;
    return ret;
}

Longitude genLongitude(int64_t time)
{
    double degree = sine(time, 0, 60 * 5 * 2.5, 5, 55) + offSim(time)*0.000012333;    
    Longitude ret;
    ret.degree = (int)degree;
    double a;
    ret.minute = 60 * modf(degree, &a);
    ret.direction = East;
    return ret;
}
