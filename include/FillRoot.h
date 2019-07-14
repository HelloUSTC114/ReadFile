#ifndef FILLROOT_H
#define FILLROOT_H
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TObject.h"
#include "TRefArray.h"
#include "TRef.h"
#include "TClonesArray.h"
#include <iostream>

#include "DataType.h"
#include "ReadOneEvent.h"

#ifdef __ROOTCLING__
#pragma link C++ class RootHeader_t;
#pragma link C++ class RootSingle_t;
#pragma link C++ class RootGroup_t;
#pragma link C++ class RootEvent_t;
#endif

class RootHeader_t : public TObject
{
public:
    RootHeader_t() = default;
    RootHeader_t(const Header_t &);
    void SetHeader(const Header_t &);
    void Clear(){fEventCounter = 0; fTTStamp = 0;}

    int fEventCounter = 0;
    int fTTStamp = 0;
ClassDef(RootHeader_t, 1)
};


class RootSingle_t : public TObject
{
    friend int ReadOneEvent(OutputFileManager &, Header_t &, RootSingle_t &);

public:
    RootSingle_t() = default;
    RootSingle_t(const SingleEvent_t &);
    
#ifdef DEBUG
    float &operator[](int i){return fData[i];}; // used only when debug
#endif

    void SetData(const SingleEvent_t &);
    
    void ClearData();   // Need lots of time, reinitialize float array.
    ostream& Show(ostream &os) const;
private:
    float fData[1024]{0};
ClassDef(RootSingle_t,1)
};


class RootGroup_t : public TObject
{
public:
    RootGroup_t();
    ~RootGroup_t();
    void ClearGroup();
    bool AddEvent(RootSingle_t &, int ch);
    RootSingle_t * GetEvent(int ch);

    ostream & PrintGroup(ostream & os, bool verbose = 1) const;

    bool AllChannelFilled(); // Judge whether all channels are filled
    bool ReadyToUse();  // if TR is needed to be filled, than judge whether TR is filled

    
private:
    bool fTRFlag = 1;
    bool fTRFilledFlag = 0;

    int fChannelFilledCounter = 0;
    int fChFilledPosition[8]; // record channel related event array position
    int fPositionRelatedChan[8]; // record event array position related channel
    TRef fRefTR;
    TRefArray fRefArray;

ClassDef(RootGroup_t, 1);
};

class RootEvent_t  : public TObject
{
public:
    RootEvent_t();
    ~RootEvent_t();

    ostream& PrintEvent(ostream &os, bool verbose = 1);
    
    RootGroup_t * GetGroup(int gr);
    RootSingle_t * GetChannel(int gr, int ch);

    bool SetGroupFlag(int gr, bool flag);
    
    bool SetHeader(const RootHeader_t&);
    bool AddGroup(RootGroup_t &, int gr);
    
    
    void Clear();   // Clear Data only, won't clear configuration
    bool AllGroupFilled();
    bool IsFilling();

private:

    int CalGroupNum();
    int fGroupNum = 0;
    bool fGroupFlag[4]{0};

    bool fHeaderFilledFlag = 0;
    RootHeader_t fHeader;

    int fGrFilledCounter = 0;
    int fGrFilledPosition[4];
    int fPositionRelatedGr[4];
    TRefArray fRefArray;    // Pointer to Groups



ClassDef(RootEvent_t, 1);
};

int ReadOneEvent(OutputFileManager &, Header_t &, RootSingle_t &);

int ConvertGrToCh(int gr, int ch);
bool ConvertChToGr(int Channel, int &gr, int &ch);

#endif