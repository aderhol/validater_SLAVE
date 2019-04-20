#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "Board.h"
#include <ti/drivers/UART.h>
#include <xdc/runtime/System.h>

#include "blogic.h"
#include "gen.h"

#define UART_BAUD 115200

#define csprintf(buff, format, ...) sprintf(buff + strlen(buff), format, ##__VA_ARGS__)

#define uprintf(format, ...)    {char _UPRINTF_buff[1000];sprintf(_UPRINTF_buff, format "\r\n", ##__VA_ARGS__);UART_write(uart, _UPRINTF_buff, strlen(_UPRINTF_buff));}

static int checksum(const char *s);

static void addTime3(char* buff, int64_t time);
static void addLatitude4(char* buff, int64_t time);
static void addLatitudeDir(char* buff, int64_t time);
static void addLongitude4(char* buff, int64_t time);
static void addLongitudeDir(char* buff, int64_t time);
static void addSpeed2(char* buff, int64_t time);
static void addTrack2(char* buff, int64_t time);
static void addDate(char* buff, int64_t time);
static void addNumberOfSatellitesUsed(char* buff, int64_t time);
static void addHDOP2(char* buff, int64_t time);
static void addVDOP2(char* buff, int64_t time);
static void addPDOP2(char* buff, int64_t time);
static void addAltitude1(char* buff, int64_t time);
static void addUndulation1(char* buff, int64_t time);
static void addUsedGPSSatellites(char* buff, int64_t time);
static void addUsedGLONASSSatellites(char* buff, int64_t time);

UART_Handle uart;

static void sendGNRMC(int64_t time);
static void sendGPGGA(int64_t time);
static void sendGNGSA(int64_t time);
static void sendGPGSV(int64_t time);
static void sendGLGSV(int64_t time);

void sendPacket(int64_t time)
{
    sendGNRMC(time);
    sendGPGGA(time);
    sendGNGSA(time);
    sendGPGSV(time);
    sendGLGSV(time);    
    uprintf("$GPTXT,01,01,02,ANTSTATUS=OPEN*2B");
    uprintf("");
}

static void sendGLGSV(int64_t time)
{
    Satellite satellites[36];
    const int count = genGLONASSSatellitesInView(time, satellites);
    const int numberOfMessages = (int)ceil(count / 4.0);

    int satIndex = 0, mesIndex = 1;
    do {
        char buff[100] = "$GLGSV";
        csprintf(buff, ",%d,%d,%.2d", numberOfMessages, mesIndex, count);
        
        int i;
        for(i = 0; (i < 4) && (satIndex < count); i++){
            csprintf(buff, ",%.2d,%.2d,%.3d,%.2d",
                     satellites[satIndex].PRN,
                     satellites[satIndex].elevation,
                     satellites[satIndex].azimuth,
                     satellites[satIndex].SNR
                     );
            satIndex++;
        }
        
        uprintf("%s*%.2X", buff, checksum(buff));
        mesIndex++;
    } while(satIndex < count);
}

static void sendGPGSV(int64_t time)
{
    Satellite satellites[36];
    const int count = genGPSSatellitesInView(time, satellites);
    const int numberOfMessages = (int)ceil(count / 4.0);

    int satIndex = 0, mesIndex = 1;
    do {
        char buff[100] = "$GPGSV";
        csprintf(buff, ",%d,%d,%.2d", numberOfMessages, mesIndex, count);
        
        int i;
        for(i = 0; (i < 4) && (satIndex < count); i++){
            csprintf(buff, ",%.2d,%.2d,%.3d,%.2d",
                     satellites[satIndex].PRN,
                     satellites[satIndex].elevation,
                     satellites[satIndex].azimuth,
                     satellites[satIndex].SNR
                     );
            satIndex++;
        }
        
        uprintf("%s*%.2X", buff, checksum(buff));
        mesIndex++;
    } while(satIndex < count);
}

static void sendGNGSA(int64_t time)
{
    char buffGPS[100] = "$GNGSA";
    csprintf(buffGPS, ",A");
    csprintf(buffGPS, ",3"); //fix mode 3D
    addUsedGPSSatellites(buffGPS, time);
    addPDOP2(buffGPS, time);
    addHDOP2(buffGPS, time);
    addVDOP2(buffGPS, time);
    uprintf("%s*%.2X", buffGPS, checksum(buffGPS));
    
    char buffGLONASS[100] = "$GNGSA";
    csprintf(buffGLONASS, ",A");
    csprintf(buffGLONASS, ",3"); //fix mode 3D
    addUsedGLONASSSatellites(buffGLONASS, time);
    addPDOP2(buffGLONASS, time);
    addHDOP2(buffGLONASS, time);
    addVDOP2(buffGLONASS, time);
    uprintf("%s*%.2X", buffGLONASS, checksum(buffGLONASS));
}

static void sendGPGGA(int64_t time)
{
    char buff[100] = "$GPGGA";
    addTime3(buff, time);
    addLatitude4(buff, time);
    addLatitudeDir(buff, time);
    addLongitude4(buff, time);
    addLongitudeDir(buff, time);
    csprintf(buff, ",2"); //quality
    addNumberOfSatellitesUsed(buff, time);
    addHDOP2(buff, time);
    addAltitude1(buff, time);
    csprintf(buff, ",M");
    addUndulation1(buff, time);
    csprintf(buff, ",M");
    csprintf(buff, ",,");
    uprintf("%s*%.2X", buff, checksum(buff));
}

static void sendGNRMC(int64_t time)
{
    char buff[100] = "$GNRMC";
    addTime3(buff, time);
    csprintf(buff, ",A");
    addLatitude4(buff, time);
    addLatitudeDir(buff, time);
    addLongitude4(buff, time);
    addLongitudeDir(buff, time);
    addSpeed2(buff, time);
    addTrack2(buff, time);
    addDate(buff, time);    
    csprintf(buff, ",,,D");
    uprintf("%s*%.2X", buff, checksum(buff));
}

void bLogic_Init(void)
{
    UART_Params uartParams;
    /* Create a UART with data processing off. */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = UART_BAUD;
    uart = UART_open(Board_UART0, &uartParams);

    if (uart == NULL) {
        System_abort("Error opening the UART");
    }
}

static void addUsedGPSSatellites(char* buff, int64_t time)
{
    int PRN[12];
    int count = genUsedGPSSatelliteList(time, PRN);
    int i;
    for(i = 0; i < 12; i++) {
        if(i < count)
            csprintf(buff, ",%.2d", PRN[i]);
        else
            csprintf(buff, ",");
    }
}

static void addUsedGLONASSSatellites(char* buff, int64_t time)
{
    int PRN[12];
    int count = genUsedGLONASSSatelliteList(time, PRN);
    int i;
    for(i = 0; i < 12; i++) {
        if(i < count)
            csprintf(buff, ",%.2d", PRN[i]);
        else
            csprintf(buff, ",");
    }
}

static void addHDOP2(char* buff, int64_t time)
{
    csprintf(buff, ",%4.2f%", genHDOP(time));
}

static void addVDOP2(char* buff, int64_t time)
{
    csprintf(buff, ",%4.2f%", genVDOP(time));
}

static void addPDOP2(char* buff, int64_t time)
{
    csprintf(buff, ",%4.2f%", genPDOP(time));
}

static void addNumberOfSatellitesUsed(char* buff, int64_t time)  
{
    csprintf(buff, ",%.2d%", genNumberOfSatellitesUsed(time));
}
static void addDate(char* buff, int64_t time)
{
    int years = (time / 31104000) % 100;
    time %= 31104000;
    int months = time / 2592000;
    time %= 2592000;
    int days = time / 86400;
    csprintf(buff, ",%.2d%.2d%.2d", days, months, years);
}

static void addTrack2(char* buff, int64_t time)
{
    csprintf(buff, ",%06.2f", genTrack(time));
}

static void addSpeed2(char* buff, int64_t time)
{
    csprintf(buff, ",%04.2f", genSpeed(time));
}

static void addLongitude4(char* buff, int64_t time)
{
    Longitude lon = genLongitude(time);
    csprintf(buff, ",%.3d%07.4f", lon.degree, lon.minute);
}

static void addLongitudeDir(char* buff, int64_t time)
{
    Longitude lon = genLongitude(time);
    switch (lon.direction){
    case East:
        csprintf(buff, ",E");
        break;
    case West:
        csprintf(buff, ",W");
    default:
        while(1);
    }
}

static void addLatitude4(char* buff, int64_t time)
{
    Latitude lat = genLatitude(time);
    csprintf(buff, ",%.2d%07.4f", lat.degree, lat.minute);
}

static void addLatitudeDir(char* buff, int64_t time)
{
    Latitude lat = genLatitude(time);
    switch (lat.direction){
    case North:
        csprintf(buff, ",N");
        break;
    case South:
        csprintf(buff, ",S");
    default:
        while(1);
    }
}

static void addTime3(char* buff, int64_t time)
{
    time %= 86400; //gets the seconds belonging to the current day
    int hours = time / 3600;
    time %= 3600; //gets the current hour
    int minutes = time / 60;
    int seconds = time % 60;
    csprintf(buff, ",%.2d%.2d%.2d.000", hours, minutes, seconds);
}

static int checksum(const char *s) {
    int c = 0;
    s++;
    while(*s)
        c ^= *s++;

    return c;
}

static void addUndulation1(char* buff, int64_t time)
{
    csprintf(buff, ",%.1f", genUndulation(time));
}

static void addAltitude1(char* buff, int64_t time)
{
    csprintf(buff, ",%.1f", genAltitude(time));
}
