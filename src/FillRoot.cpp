#include "FillRoot.h"
#include <memory>

ClassImp(RootHeader_t);
ClassImp(RootSingle_t);

ClassImp(RootEvent_t);

// TClonesArray *RootEvent_t::fgSingles = new TClonesArray("RootSingle_t", 40);

RootHeader_t::RootHeader_t(const Header_t &header)
{
    fEventCounter = header.fEventCounter;
    fTTStamp = header.fTriggerTimeTag;
}

RootSingle_t::RootSingle_t(const SingleEvent_t &event)
{
    memcpy(fData, event.Data, 1024 * sizeof(float));
}

ostream & RootSingle_t::Show(ostream &os)
{
    os << "-------Channel details: ------" << endl;
    for (int i = 0 ; i < 1024; i++)
    {
        os << fData[i] << '\t';
    }
    os << endl << "-----------The End----------" << endl;
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

RootEvent_t::RootEvent_t()
{
}

RootEvent_t::~RootEvent_t()
{
}

bool RootEvent_t::AddEvent(RootSingle_t& event, int gr, int ch)
{
    if(ch == 8)
    {
        if(gr < 3)
        {
            if(fTR0FilledFlag)
                return false;
            fRefTR0 = &event;
            fTR0FilledFlag = 1;
        }
        else
        {
            if(fTR1FilledFlag)
                return false;
            fRefTR1 = &event;
            fTR1FilledFlag = 1;
        }
        
    }
    else
    {
        if(fChannelFillingCounter != 8*gr+ch)
            return false;
        fRefArray.Add(&event);
        fChannelFillingCounter++;
        fChanFilledFlag[8*gr+ch] = 1;
    }
    return true;
    
}

ostream& RootEvent_t::PrintEvent(ostream& os, bool verbose)
{
    int counter = 0;
    for(int i = 0; i < 32; i++)
    {
        if(fChanFilledFlag[i])
        {
            counter ++;
        }
    }
    if(fTR0FilledFlag)  counter++;
    if(fTR1FilledFlag)  counter++;

    os << "There're " << CalTotalChannelNum() << " channel(s) in total, " << counter << " channel(s) already saved." << endl;
    
    if(verbose)
    {
        for(int i = 0; i < 32; i++)
        {
            if(fChanFilledFlag[i])
            {
                os << "Channel: " << i << endl;
                ((RootSingle_t*)fRefArray.At(i) )-> Show(os);
            }
            
        }
        if(fTR0FilledFlag)
        {
            os << "TR0: " << endl;
            ((RootSingle_t *)fRefTR0.GetObject())->Show(os);
        }
        if (fTR1FilledFlag)
        {
            os << "TR0: " << endl;
            ((RootSingle_t *)fRefTR1.GetObject())->Show(os);
        }
    }
}