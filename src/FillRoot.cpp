#include "FillRoot.h"
#include <memory>

ClassImp(RootHeader_t);
ClassImp(RootSingle_t);
ClassImp(RootGroup_t);
ClassImp(RootEvent_t);

// TClonesArray *RootEvent_t::fgSingles = new TClonesArray("RootSingle_t", 40);

RootHeader_t::RootHeader_t(const Header_t &header)
{
    SetHeader(header);
}

void RootHeader_t::SetHeader(const Header_t& header)
{
    fEventCounter = header.fEventCounter;
    fTTStamp = header.fTriggerTimeTag;
}

RootSingle_t::RootSingle_t(const SingleEvent_t &event)
{
    SetData(event);
}

void RootSingle_t::SetData(const SingleEvent_t &event)
{
    memcpy(fData, event.Data, 1024 * sizeof(float));
}

void RootSingle_t::ClearData()
{
    for(int i = 0; i < 1024; i++)
    {
        fData[i] = 0;
    }
}

ostream & RootSingle_t::Show(ostream &os) const
{
    os << "-------Channel details: ------" << endl;
    for (int i = 0 ; i < 1024; i++)
    {
        os << fData[i] << '\t';
    }
    os << endl << "-----------The End----------" << endl;
}

// ------------------------RootGruop--------------

RootGroup_t::RootGroup_t()
{
    ClearGroup();
}

RootGroup_t::~RootGroup_t()
{
    ClearGroup();
}

void RootGroup_t::ClearGroup()
{
    fTRFilledFlag = 0;
    for(int i = 0; i < 8; i++)
    {
        fChFilledPosition[i] = -1;
        fPositionRelatedChan[i] = -1;
    }
    fRefTR.Clear();
    fRefArray.Clear();
}

bool RootGroup_t::AddEvent(RootSingle_t & event, int ch)
{
    if(ch == 8)
    {
        if(fTRFilledFlag)
            return false;
        fRefTR = &event;
        fTRFilledFlag = 1;
    }
    else if(ch < 8 && ch >= 0)
    {
        if(fChFilledPosition[ch] != -1)
        {
            return false;
        }
        fRefArray.Add(&event);
        fChFilledPosition[ch] = fChannelFilledCounter;
        fPositionRelatedChan[fChannelFilledCounter] = ch;
        fChannelFilledCounter++;
    }
    return true;
}

RootSingle_t * RootGroup_t::GetEvent(int ch)
{
    if(ch == 8)
    {
        return (RootSingle_t*)(fRefTR.GetObject());
    }
    else if(ch >=0 && ch < 8)
    {
        int tPosition = fChFilledPosition[ch];
        if(tPosition < 0)   return NULL;
        return (RootSingle_t*) fRefArray . At(tPosition);
    }
    return NULL;
}

bool RootGroup_t::AllChannelFilled()
{
    bool result = fTRFilledFlag;
    if(!result)
    {
        return false;
    }
    if(fChannelFilledCounter != 8)
    {
        return false;
    }
    return true;
}

bool RootGroup_t::ReadyToUse()
{
    return fTRFilledFlag>=fTRFlag;
}

ostream & RootGroup_t::PrintGroup(ostream& os, bool verbose) const
{
    os << "TR Flag is " << fTRFlag << ", TR Filled flag is: " << fTRFilledFlag << endl;
    os << "There're 8 ch in total, " << fChannelFilledCounter << " has filled" << endl;

    if(verbose)
    {
        if(fTRFilledFlag)
        {
            os << "Channel TR: " << endl;
            ((RootSingle_t*)fRefTR.GetObject()) -> Show(os);
        }
        for(int i = 0; i < 8; i++)
        {
            int tPosition = fChFilledPosition[i];
            if(tPosition != -1)
            {
                os << "Channel: " << i << endl;
                ((RootSingle_t*)fRefArray.At(tPosition)) -> Show(os);
            }
        }
    }
}
// ----------------------RootEvent-----------------

RootEvent_t::RootEvent_t()
{
    Clear();
}

RootEvent_t::~RootEvent_t()
{
    Clear();
}

int RootEvent_t::CalGroupNum()
{
    int counter = 0;
    for(int i = 0; i < 4; i++)
    {
        if(fGroupFlag[i])
        {
            counter ++;
        }
    }
    fGroupNum = counter;
    return fGroupNum;
}

bool RootEvent_t::AddGroup(RootGroup_t & group, int gr)
{
    if(fGrFilledPosition[gr] != -1)
    {
        return false;
    }
    fRefArray.Add(&group);
    fGrFilledPosition[gr] = fGrFilledCounter;
    fPositionRelatedGr[fGrFilledCounter] = gr;
    fGrFilledCounter++;
    return true;
}

ostream& RootEvent_t::PrintEvent(ostream& os, bool verbose)
{
    os << "There're " << fGroupNum << " groups in total, " << fGrFilledCounter << " has been filled yet" << endl;
    for(int i = 0; i < 4; i++)
    {
        os << "Group: " << i << endl;
        int tPosition = fGrFilledPosition[i];
        if(tPosition != -1)
        {
            ((RootGroup_t*)fRefArray.At(tPosition)) -> PrintGroup(os, verbose);
        }
    }
    return os;
}

bool RootEvent_t::AllGroupFilled()
{
    for(int i = 0; i < 4; i++)
    {
        if(fGroupFlag[i])
        {
            if(fGrFilledPosition[i] == -1)
                return false;
        }
        else
        {
            if(fGrFilledPosition[i] != -1)
            {
                cerr << "Warning: Group " << i << " is not needed, but already filled yet" << endl;
            }
        }
        
    }
    return true;
}


RootGroup_t * RootEvent_t::GetGroup(int gr)
{
    if(gr < 0 || gr >3)
    {
        return NULL;
    }
    int tPosition = fGrFilledPosition[gr];
    if(tPosition < 0)   return NULL;
    return (RootGroup_t *) fRefArray .At(tPosition);
}

RootSingle_t * RootEvent_t::GetChannel(int gr, int ch)
{
    auto group = GetGroup(gr);
    if(!group)  return NULL;
    return group -> GetEvent(ch);
}

bool RootEvent_t::SetGroupFlag(int gr, bool flag)
{
    if(IsFilling())
    {
        return false;
    }    
    fGroupFlag[gr] = flag;
    return true;
}

bool RootEvent_t::IsFilling()
{
    if(fGrFilledCounter != 0)
    {
        return false;
    }
    return true;
}

void RootEvent_t::Clear()
{
    for(int i = 0; i < 4; i++)
    {
        fGrFilledPosition[i] = -1;
        fPositionRelatedGr[i] = -1;
    }
    fHeaderFilledFlag = 0;
    fHeader.Clear();
    fRefArray.Clear();
}

bool RootEvent_t::SetHeader(const RootHeader_t & header)
{
    if(fHeaderFilledFlag)   return false;
    fHeader = header;
    return true;
}


int ConvertGrToCh(int gr, int ch)
{
    if (ch >= 8 || ch < 0)
    {
        return -1;
    }
    if (gr >= 4 || gr < 0)
    {
        return -1;
    }

    return gr * 8 + ch;
}

bool ConvertChToGr(int Channel, int &gr, int &ch)
{
    if (Channel < 0 || Channel >= 36)
    {
        return 0;
    }
    gr = Channel / 8;
    ch = Channel % 8;
    return true;
}


int ReadOneEvent(OutputFileManager & manager, Header_t & header, RootSingle_t &single)
{
    manager . ReadOneEvent(header, single.fData);
}