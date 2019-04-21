#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "gen.h"

#include "waveforms.h"

double genHDOP(int64_t time)
{
    return square(time, 90, 3600*1, 50, 0.3, 0.5);
}
double genVDOP(int64_t time)
{
    return triangle(time, 90, 3600*1, 0.3, 0.5);
}

double genPDOP(int64_t time)
{
    return sine(time, 90, 3600*1, 0.3, 0.5);
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
    
    int maxSNR = satellites[0].SNR = (int)(triangle(time, 90, 3600*2, 15, 75) + 0.5);
    double avrSNR = sine(time, 90, 3600*3, 10, 40);
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
    
    int maxSNR = satellites[0].SNR = (int)(triangle(time, 180, 3600*2, 20, 75) + 0.5);
    double avrSNR = sine(time, 180, 3600*3, 15, 35);
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
    return sawtooth(time, 90, 60 * 3, 100, 1500);
}

double genUndulation(int64_t time)
{
    return -16.5;
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
    double degree = sine(time, 0, 60 * 5 * 3.2, 5, 55);    
    Latitude ret;
    ret.degree = (int)degree;
    double a;
    ret.minute = 60 * modf(degree, &a);
    ret.direction = North;
    return ret;
}

Longitude genLongitude(int64_t time)
{
    double degree = sine(time, 0, 60 * 5, 5, 55);    
    Longitude ret;
    ret.degree = (int)degree;
    double a;
    ret.minute = 60 * modf(degree, &a);
    ret.direction = East;
    return ret;
}
