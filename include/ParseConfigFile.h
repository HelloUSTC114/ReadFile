#ifndef PARSECONFIGFILE_H
#define PARSECONFIGFILE_H 1

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <math.h>
#include <cstring>
// #include <CAENDigitizer.h>
// #include <CAENDigitizerType.h>
#include "CAENType.h"

// #include <unistd.h>
// #include <ctype.h>
// #include <sys/time.h>

#define MAX_CH 64
#define MAX_SET 16  /* max. number of independent settings */
#define MAX_GW 1000 /* max. number of generic write commands */

#define GNUPLOT_DEFAULT_PATH "/usr/bin/"

#define CFGRELOAD_CORRTABLES_BIT (0)
#define CFGRELOAD_DESMODE_BIT (1)

using namespace std;

typedef enum {
    OFF_BINARY = 0x00000001,    // Bit 0: 1 = BINARY, 0 = ASCII
    OFF_HEADER = 0x00000002,    // Bit 1: 1 = include header, 0 = just samples data
} OUTFILE_FLAGS;

typedef struct{
    float cal[MAX_SET];
    float offset[MAX_SET];
} DAC_Calibration_data;

struct WaveDumpConfig_t
{
public:
    int LinkType;
    int LinkNum;
    int ConetNode;
    uint32_t BaseAddress;
    int Nch;
    int Nbit;
    float Ts;
    int NumEvents;
    uint32_t RecordLength;
    int PostTrigger;
    int InterruptNumEvents;
    int TestPattern;
    CAEN_DGTZ_EnaDis_t DesMode;
    //int TriggerEdge;
    CAEN_DGTZ_IOLevel_t FPIOtype;
    CAEN_DGTZ_TriggerMode_t ExtTriggerMode;
    uint16_t EnableMask;
    char GnuPlotPath[1000];
    CAEN_DGTZ_TriggerMode_t ChannelTriggerMode[MAX_SET];
    CAEN_DGTZ_PulsePolarity_t PulsePolarity[MAX_SET];
    uint32_t DCoffset[MAX_SET];
    int32_t DCoffsetGrpCh[MAX_SET][MAX_SET];
    uint32_t Threshold[MAX_SET];
    int Version_used[MAX_SET];
    uint8_t GroupTrgEnableMask[MAX_SET];
    uint32_t MaxGroupNumber;

    uint32_t FTDCoffset[MAX_SET];
    uint32_t FTThreshold[MAX_SET];
    CAEN_DGTZ_TriggerMode_t FastTriggerMode;
    uint32_t FastTriggerEnabled;
    int GWn;
    uint32_t GWaddr[MAX_GW];
    uint32_t GWdata[MAX_GW];
    uint32_t GWmask[MAX_GW];
    OUTFILE_FLAGS OutFileFlags;
    uint16_t DecimationFactor;
    int useCorrections;
    int UseManualTables;
    char TablesFilenames[MAX_X742_GROUP_SIZE][1000];
    CAEN_DGTZ_DRS4Frequency_t DRS4Frequency;
    int StartupCalibration;
    DAC_Calibration_data DAC_Calib;

};

// void SetDefaultConfiguration(WaveDumpConfig_t *WDcfg);   // static function, need not to be seen in other files


int ParseConfigFile(FILE *f_ini, WaveDumpConfig_t *WDcfg);
// int ParseConfigFile(string fFileName, WaveDumpConfig_t * WDcfg);

#endif