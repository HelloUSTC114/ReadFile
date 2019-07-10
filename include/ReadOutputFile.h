#ifndef READOUTPUTFILE_H
#define READOUTPUTFILE_H 1

#include <fstream>
#include "ParseConfigFile.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"


struct EventInfo_t
{
    uint32_t EventSize;
    uint32_t BoardId;
    uint32_t Pattern;
    uint32_t ChannelMask;
    uint32_t EventCounter;
    uint32_t TriggerTimeTag;
};

struct GroupEvent_t
{
    uint32_t ChSize[MAX_X742_CHANNEL_SIZE];    //[9] the number of samples stored in DataChannel array
    float *DataChannel[MAX_X742_CHANNEL_SIZE]; //[9]-> the array of ChSize samples
    uint32_t TriggerTimeTag;
    uint16_t StartIndexCell;
};

struct Event_t
{
    bool GrPresent[MAX_X742_GROUP_SIZE];    //[4] if the group has data, the value is 1, otherwise is 0
    GroupEvent_t DataGroup[MAX_X740_GROUP_SIZE];    //[4] array of chSize samples
};

struct ReadEvent_t
{
    EventInfo_t fEventInfo;
    float fData[1024];  //[1024] only one event in one channel;
};

struct Header_t
{
    uint32_t fLength;
    uint32_t fBoardID;
    uint32_t fPattern;
    uint32_t fChannel;
    uint32_t fEventCounter;
    uint32_t fTriggerTimeTag;
};

struct SingleEvent_t
{
    float Data[1024];
};

void FillZeroHeader(Header_t &header);

void FillZeroEvent(SingleEvent_t &event);

class OutputFileManager
{
public:
    OutputFileManager(string sfile);
    OutputFileManager(string sfile, int gr, int ch);
    void ConvertFileFlag(OUTFILE_FLAGS);
    bool OpenFile(string s);
    int ReadOneEvent(Header_t &, SingleEvent_t &);
private:
    string fFileName;
    ifstream fStream;

    int fGroup;
    int fChannel;
    bool fTRFlag;
    bool fBinaryFlag;   // 0: ASCII, 1: Binary
    bool fHeaderFlag;   // 0: No header, 1: Header

    int fEventCounter = 0;  // Eventcounter

    bool fFileNameParsed = 0;
    bool fFileHeaderParsed = 0;
    bool ParseDataFileName(string sfile);
    bool ParseDataFileHeader(string sfile);

    void FillHeaderManually(Header_t &header);

    bool OpenFile();
};




#endif