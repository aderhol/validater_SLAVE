#ifndef GEN_H
#define GEN_H

#include <stdint.h>
#include <stdbool.h>

int genUsedGLONASSSatelliteList(int64_t time, int* PRN); //returns the number of used GLONASS satellites
int genUsedGPSSatelliteList(int64_t time, int* PRN); //returns the number of used GPS satellites
double genUndulation(int64_t time);
double genAltitude(int64_t time);
double genHDOP(int64_t time);
double genVDOP(int64_t time);
double genPDOP(int64_t time);
int genNumberOfSatellitesUsed(int64_t time);
double genSpeed(int64_t time);
double genTrack(int64_t time);

typedef struct{
    int PRN;
    int elevation;
    int azimuth;
    int SNR;
    bool used;
}Satellite;
int genGPSSatellitesInView(int64_t time, Satellite* satellites);
int genGLONASSSatellitesInView(int64_t time, Satellite* satellites);

typedef enum {
    North,
    South
}LatitudeDirection;

typedef struct {
    int degree;
    double minute;
    LatitudeDirection direction;
}Latitude;

Latitude genLatitude(int64_t time);

typedef enum {
    East,
    West
}LongitudeDirection;

typedef struct {
    int degree;
    double minute;
    LongitudeDirection direction;
}Longitude;

Longitude genLongitude(int64_t time);

#endif
