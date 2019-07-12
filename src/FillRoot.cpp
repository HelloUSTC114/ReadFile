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
    for(int i = 0; i < 32; i++)
    {
        fChFilledPosition[i] = -1;  // Mark all position as non filled yet.
        fPositionRelatedChan[i] = -1;   // Mark all position as non fille yet.
    }
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
    else if(ch < 8)
    {
        int tCh = ConvertGrToCh(gr, ch);
        if(tCh == -1)   return false;
        if(fChFilledPosition[tCh] != -1)
        {
            return false;
        }
        // Add Event to chanenl, and record position 
        fChannelFillingCounter++;
        fRefArray.Add(&event);
        fChFilledPosition[tCh] = fChannelFillingCounter;
        fPositionRelatedChan[fChannelFillingCounter] = tCh;
    }
    return true;
    
}

ostream& RootEvent_t::PrintEvent(ostream& os, bool verbose)
{
    int counter = 0;
    for(int i = 0; i < 32; i++)
    {
        if(fChFilledPosition[i])
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
            if(fChFilledPosition[i])
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

bool RootEvent_t::ReadyToSave()
{
    bool tResult = (fTR0Flag <= fTR0FilledFlag) && (fTR1Flag <= fTR1FilledFlag);
    if(!tResult)    return false;

    bool tStrict = (fTR0Flag == fTR0FilledFlag) && (fTR1Flag == fTR1FilledFlag);
    if(tStrict <= tResult)
    {
        cerr << "Warning! Fill more data than expected!" << endl;
        cerr << "TR0: Config Flag: " << fTR0Flag << " Filled Flag: " << fTR0FilledFlag;
        cerr << "TR1: Config Flag: " << fTR1Flag << " Filled Flag: " << fTR1FilledFlag;
    }

    for(int gr = 0; gr < 4; gr++)
    {
        if(fGroupFlag[gr])
        {
            for(int ch = 0; ch < 8; ch++)
            {
                int tCh = ConvertGrToCh(gr, ch);
                if(fChFilledPosition[tCh] < 0)  return false;   // judge each channel position
            }
        }
        else
        {
            for(int ch = 0; ch < 8; ch++)
            {
                int tCh = ConvertGrToCh(gr, ch);
                if(fChFilledPosition[tCh] >=0)
                {
                    cerr << "Warning! Fill more data than expected!" << endl;
                    cerr << "Group: " << gr << " Channel: " << ch << endl;
                    cerr << "Group Flag: " << fGroupFlag[gr] << endl;
                    cerr << "Data Position: " << fChFilledPosition[tCh] << endl;
                }
            }
        }
        
    }
}

int ConvertGrToCh(int gr, int ch)
{
    if(ch >= 8 || ch < 0)
    {
        return -1;
    }
    if(gr >=4 || gr < 0)
    {
        return -1;
    }

    return gr * 8 + ch;
}

bool ConvertChToGr(int Channel, int &gr, int &ch)
{
    if(Channel < 0 || Channel >= 36)
    {
        return 0;
    }
    gr = Channel / 8;
    ch = Channel % 8;
    return true;
}

RootSingle_t * RootEvent_t::GetChannel(int gr, int ch)
{
    if(ch == 8)
    {
        if(gr < 3)
        {
            return (RootSingle_t*)(fRefTR0.GetObject());
        }
        else if(gr == 3)
        {
            return (RootSingle_t*)(fRefTR1.GetObject());
        }
    }
    else
    {
        int tCh = ConvertGrToCh(gr, ch);
        if(tCh < 0) return NULL;
        int tPosition = fChFilledPosition[tCh];
        if(tPosition < 0)   return NULL;
        return (RootSingle_t*)fRefArray.At(tPosition);
    }
    return NULL;
    
}

bool RootEvent_t::SetGroupFlag(int gr, bool flag)
{
    
}