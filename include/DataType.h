#ifndef DATATYPE_H
#define DATATYPE_H

#include "CAENType.h"
#include "TObject.h"

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

#endif