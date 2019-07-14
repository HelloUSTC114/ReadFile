#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H
#include "TFile.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TSystem.h"
#include <string>
#include "FillRoot.h"
#include "ReadOneEvent.h"

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

    bool IsReading() const {return fReadFlag;}
    bool IsWriting() const {return fWriteFlag;}
    bool IsFunctioning() const {return fReadFlag||fWriteFlag;}

    bool ProcessOneEvent(); // Only useful when writing events into root file(fill Tree)

private:
    string fFileName;
    TFile *fFile = NULL;
    TTree *fTree = NULL;
    TBranch *fBranch = NULL;
    bool TreeInitiate();

    bool fWriteFlag = 0;    // Record read/write status
    bool fReadFlag = 0; // Record read/write status

    // -----------Only useful while reading raw data------------
    Header_t fHeader;
    RootHeader_t *fRootHeader = NULL;
    RootGroup_t *fRootGroup[4]{0};
    bool fGroupFlag[4]{0};

    RootEvent_t *fRootEvent = NULL;

    
    TClonesArray *fSingleEventArray = NULL; // Single event array, save singles events for each channel, to avoid construct/destruct single event repeatly    

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