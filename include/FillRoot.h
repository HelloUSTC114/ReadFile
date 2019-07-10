#ifndef FILLROOT_H
#define FILLROOT_H
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TObject.h"
#include "TRefArray.h"

#include "DataType.h"
#include "ReadOneEvent.h"

#ifdef __ROOTCLING__
#pragma link C++ class RootHeader_t;
#pragma link C++ class RootSingle_t;
#endif

class RootHeader_t : public TObject
{
public:
    RootHeader_t() = default;
    RootHeader_t(const Header_t &);

    int fEventCounter = 0;
    int fTTStamp = 0;
ClassDef(RootHeader_t, 1)
};

class RootSingle_t : public TObject
{
public:
    RootSingle_t() = default;
    RootSingle_t(SingleEvent_t &);
    float *fData;   //[1024]
ClassDef(RootSingle_t,1)
};

class RootEvent_t  : public TObject
{
public:
    RootEvent_t();
    RootEvent_t(int ch);
    ~RootEvent_t();

    int GetTotalChannelNum(){return fChannelNum;}
private:
    bool fHeaderFlag;

    bool fGroupFlag[4];
    bool fTR0Flag;
    bool fTR1Flag;

    int fChannelNum = 0;    //!
    int CalTotalChannelNum();

    void GenerateArray();
    void Clear();
ClassDef(RootEvent_t, 1);
};
#ifdef __ROOTCLING__
#pragma link C++ class RootEvent_t;
#endif

class ProcessRawData
{
public:
    ProcessRawData();
    ~ProcessRawData();
private:
    TFile *fFile;
    TTree *fTree;
    
};

#endif