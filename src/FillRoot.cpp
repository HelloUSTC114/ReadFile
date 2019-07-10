#include "FillRoot.h"

ClassImp(RootHeader_t);
ClassImp(RootSingle_t);

ClassImp(RootEvent_t);

RootHeader_t::RootHeader_t(const Header_t &header)
{
    fEventCounter = header.fEventCounter;
    fTTStamp = header.fTriggerTimeTag;
}

RootSingle_t::RootSingle_t(SingleEvent_t &event)
{
    fData = event.Data;
}

int RootEvent_t::CalTotalChannelNum()
{
    int tCount = 0;
    for(int i = 0; i < 4; i++)
    {
        if(fGroupFlag[i])
        {
            tCount += 4;
        }
    }
    if(fTR0Flag)
    {
        tCount ++;
    }
    if(fTR1Flag)
    {
        tCount ++;
    }
    fChannelNum = tCount;
    return fChannelNum;
}