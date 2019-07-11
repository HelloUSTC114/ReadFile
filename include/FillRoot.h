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
#pragma link C++ class RootEvent_t;
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
    RootSingle_t(const SingleEvent_t &);
    float fData[1024]{0};
    ostream& Show(ostream &os);
ClassDef(RootSingle_t,1)
};

class RootEvent_t  : public TObject
{
public:
    RootEvent_t();
    ~RootEvent_t();

    bool AddEvent(RootSingle_t&, int gr, int ch);
    ostream& PrintEvent(ostream &os, bool verbose = 1);
    
    int GetTotalChannelNum(){return fChannelNum;}
    
private:
    bool fHeaderFlag;

    bool fGroupFlag[4];
    bool fTR0Flag;
    bool fTR1Flag;

    int fChannelNum = 0;
    int CalTotalChannelNum();

    int fChannelFillingCounter = 0; //!
    bool fChanFilledFlag[32]{0}; 
    bool fTR0FilledFlag = 0;    
    bool fTR1FilledFlag = 0;    


    // static TClonesArray *fgSingles; //! Used as save data temporaryly
    TRefArray fRefArray;
    TRef fRefTR0;
    TRef fRefTR1;


ClassDef(RootEvent_t, 1);
};

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