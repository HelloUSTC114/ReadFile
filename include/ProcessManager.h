#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TClonesArray.h"
#include "TSystem.h"
#include <string>
#include "FillRoot.h"
#include "ReadOneEvent.h"


class RootHeader_t;
class RootSingle_t;
class RootEvent_t;
class RootGroup_t;

class ProcessManager
{
public:
    ProcessManager() = default;
    ProcessManager(string filename);    // Read tree data mode
    ProcessManager(string filename, bool binflag);  // Write raw data into tree mode
    ~ProcessManager();
    void Clear();
    bool SetFile(string filename, bool ReadFlag = 1);   // ReadFlag: 0 for write, 1 for read
    bool SetBinFlag(bool binflag);
    bool GetBinFlag(){return fBinFlag;}

    RootEvent_t *GetEvent(){return gRootEvent;}

    bool IsReading() const {return fReadFlag;}
    bool IsWriting() const {return fWriteFlag;}
    bool IsFunctioning() const {return fReadFlag||fWriteFlag;}

    bool FillOneEvent(); // Only useful when writing events into root file(fill Tree)
    bool GetOneEvent(int entry);    // Only useful when reading events from root file
    RootEvent_t* ReturnOneEvent(int entry);

    bool WriteTree();

    static RootEvent_t* &GetGlobalEvent();
    static RootGroup_t* &GetGlobalGroup(int gr);
    static RootSingle_t* &GetGlobalSingle(int gr, int ch);

private:
    string fFileName;
    TFile *fFile = NULL;
    TTree *fTree = NULL;

    TBranch *fEventBr = NULL;
    TBranch *fGroupBr = NULL;
    TBranch *fSingleBr = NULL;

    static bool gInitiatedStatic;
    static RootEvent_t *gRootEvent;
    static RootGroup_t *gRootGroup[4];
    static RootSingle_t *gSingleEvent[4][9];
    static void InitiateStatic();

    bool TreeInitiate();

    bool fWriteFlag = 0;    // Record read/write status
    bool fReadFlag = 0; // Record read/write status

    // -----------Only useful while reading raw data------------
    Header_t fHeader;
    RootHeader_t *fRootHeader = NULL;
    bool fGroupFlag[4]{0};


    
    // TClonesArray *fSingleEventArray = NULL; // Single event array, save singles events for each channel, to avoid construct/destruct single event repeatly    

    bool fBinFlag = 0;
    bool fFileManagerInitiated = 0;
    OutputFileManager *fFileManager[4][9]{0};
    bool FileInitiateByConfig(string sfilename);
    bool FileInitiateByList();
    void ClearFileManager();

    bool ReadOneRaw();
    bool FillOneRaw();
};

#endif