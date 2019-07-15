#include "ProcessManager.h"


ProcessManager::ProcessManager(string filename) // Read tree mode
{
    SetFile(filename, 1);   // Read file mode
}

ProcessManager::ProcessManager(string filename, bool binflag)
{
    SetFile(filename, 0);
    if(fWriteFlag)
    {
        fBinFlag = binflag;
        FileInitiateByList();
    }
}

bool ProcessManager::SetFile(string filename, bool ReadFlag)
{
    Clear();
    if(ReadFlag)
    {
        fFile = new TFile(filename.c_str());
        fTree = (TTree*) fFile -> Get("EventTree");
        if(!fTree)
        {
            cerr << "Error! Cannot find Tree in file: " << filename << endl;
            fFile -> Close();
        }
    }
    else
    {
        fFile = new TFile(filename.c_str(), "recreate");
        fTree = new TTree("EventTree", "Tree for Digitizer data");
        // fSingleEventArray = new TClonesArray("RootSingle_t", 40);
    }

    if(fFile -> IsOpen())
    {
        fFileName = filename;
        fWriteFlag = !ReadFlag;
        fReadFlag = ReadFlag;

        fRootHeader = new RootHeader_t;
        gRootEvent = new RootEvent_t;

        for(int i = 0; i < 4; i++)
        {
            gRootGroup[i] = new RootGroup_t;
        }

        TreeInitiate();
    }
    else
    {
        Clear();
    }

    ClearFileManager();
    
    
}

bool ProcessManager::SetBinFlag(bool binflag)
{
    if(!fWriteFlag)
    {
        cerr << "Error! File is not writing!" << endl;
        return false;
    }
    if(fFileManagerInitiated)
    {
        cerr << "Error! FileManager has been created, cannot channge before clear" << endl;
        return false;
    }
    fBinFlag = binflag;
    return true;
}

ProcessManager::~ProcessManager()
{
    Clear();
}

void ProcessManager::Clear()
{
    fFileName.clear();

    if (fFile)  // If File is open, Judge whether fTree exists
    {
        if (fTree)      // If fTree exists, Judge whether whether fTree is under charge of fFile
        {
            if (fTree->GetDirectory() == (TDirectory *)fFile) // if Yes, just close fFile, set fFile and fTree to NULL
            {
                // cout << "fTree is under charge of fFile" << endl;
            }
            else // If No, Save tree first, then delete tree, close file, set fFile and fTree to NULL
            {
                if (fTree->GetDirectory()->IsWritable())
                {
                    fTree->Write();
                }
                fTree->Clear();
                fTree->Delete();
            }
            fTree = NULL;
        }

        fFile->Close();
        delete fFile;
        fFile = NULL;
    }

    fWriteFlag = 0;
    fReadFlag = 0;

    // if(fSingleEventArray)
    // {
        // fSingleEventArray -> Clear();
        // fSingleEventArray -> Delete();   // Cannot use this delete function, because this function won't release this memory
        // delete fSingleEventArray;
        // fSingleEventArray = NULL;
    // }

    delete fRootHeader;
    delete gRootEvent;
    gRootEvent = NULL;
    fRootHeader = NULL;

    for(int i = 0; i < 4; i++)
    {
        delete gRootGroup[i];
        gRootGroup[i] = NULL;
        fGroupFlag[i] = 0;
    }

    fEventBr = NULL;
    fGroupBr = NULL;
    fSingleBr = NULL;


    ClearFileManager();
    
}

bool ProcessManager::TreeInitiate()
{
    if(!fTree)
    {
        return false;
    }
    if(fWriteFlag)
    {
        fEventBr = fTree -> Branch("EventBranch", &gRootEvent);
        fGroupBr = fTree -> Branch("GroupBranch", &gRootGroup[0]);
        fSingleBr = fTree -> Branch("SingleBranch", &gSingleEvent[0][0]);
    }
    else if(fReadFlag)
    {
        fEventBr = fTree -> GetBranch("EventBranch");
        fGroupBr = fTree -> GetBranch("GroupBranch");
        fSingleBr = fTree -> GetBranch("SingleBranch");

        fEventBr -> SetAddress(&gRootEvent);
    }
    return true;

}

bool ProcessManager::FillOneEvent()
{
    if(!fWriteFlag)
        return false;
    bool temp = ReadOneRaw();
    if(!temp)   return false;
    FillOneRaw();
    // cout << "Group: " << 0 << "Ch: TR " << gRootEvent->GetChannel(0, 8)->fData[0] << endl;
    // cout << "Group: " << 1 << "Ch: TR " << gRootEvent->GetChannel(1, 8)->fData[0] << endl;
    return true;
}

bool ProcessManager::FillAllEvent()
{
    int i = 0;
    for(;;)
    {
        if(!FillOneEvent())
        {
            break;
        }
        i++;
    }
    if(i == 0)  return false;
    return true;
}

bool ProcessManager::GetOneEvent(int entry)
{
    if(!fReadFlag)
    {
        cerr << "Error! Reading event from file failed, file is not on read mode" << endl;
        return false;
    }
    fEventBr -> SetAddress(&gRootEvent);
    int temp = fEventBr -> GetEntry(entry);
    if(!temp)    return false;

    for(int i = 0; i < gRootEvent -> fGroupNum; i++)
    {
        fGroupBr -> SetAddress(&gRootGroup[i]);
        
        // cout << "Event: group start index" << endl;
        // cout << gRootEvent -> fGroupWriteStartIndex << endl;
        
        temp = fGroupBr -> GetEntry(gRootEvent->fGroupWriteStartIndex + i);
        if(!temp)   return false;
        for(int j = 0; j < gRootGroup[i] -> fChannelNum + 1; j++)
        {
            // cout << "Group: Single start index: " << endl;
            // cout << gRootGroup[i] -> fSingleWriteStartIndex << endl;

            fSingleBr -> SetAddress(&gSingleEvent[i][j]);
            temp = fSingleBr -> GetEntry(gRootGroup[i] -> fSingleWriteStartIndex + j);
            if(!temp) return false;
        }
    }

    // cout << "Group: " << 0 << "Ch: TR " << gRootEvent->GetChannel(0, 8)->fData[0] << endl;
    // cout << "Group: " << 1 << "Ch: TR " << gRootEvent->GetChannel(1, 8)->fData[0] << endl;

    return true;
}

RootEvent_t * ProcessManager::ReturnOneEvent(int entry)
{
    bool test = GetOneEvent(entry);
    if(!test) return NULL;
    return gRootEvent;
}

bool ProcessManager::WriteTree()
{
    if(fWriteFlag)
    {
        fTree -> Write();
        return true;
    }
    return false;
    
}

bool ProcessManager::FillOneRaw()
{
    if(!fWriteFlag)
    {
        return false;
    }

    int &tGroupStartIndex = RootGroup_t::GetGlobalGroupWriteCounter();
    gRootEvent->fGroupWriteStartIndex = tGroupStartIndex;

    for(int gr = 0; gr < 4; gr ++)
    {
        if(!fGroupFlag[gr])     // This group is marked as no data
            continue;

        int &tSingleStartIndex = RootSingle_t::GetGlobalSinglerWriteCounter();  // Record group index
        gRootGroup[gr]->fSingleWriteStartIndex = tSingleStartIndex;

        // cout << "****Group: " << gr << endl;
        // cout << "Group start index: " << tSingleStartIndex << endl;

        for(int ch = 0; ch < 9; ch++)
        {
            if(!fFileManager[gr][ch])   // This channel has no file manager
                continue;

            gRootGroup[gr] -> AddEvent(*gSingleEvent[gr][ch], ch);

            fSingleBr -> SetAddress(&gSingleEvent[gr][ch]);
            fSingleBr -> Fill();

            // if (  ch == 8)
            // {
            //     cout << "gr: " << gr << " ch: " << ch << endl;
            //     cout << "Test:::::::::::::::::::" << endl;
            //     cout << gRootGroup[gr] -> GetEvent(ch) ->fData[0] << endl;
            //     cout << "John" << endl;
            // }

            tSingleStartIndex ++;
            
            
        }
        gRootEvent -> AddGroup(*gRootGroup[gr], gr);

        fGroupBr -> SetAddress(&gRootGroup[gr]);
        fGroupBr -> Fill();

        tGroupStartIndex ++;
    }

    fEventBr -> SetAddress(&gRootEvent);
    fEventBr -> Fill();
    return true;
}

bool ProcessManager::ReadOneRaw()
{
    for (int gr = 0; gr < 4; gr++)
    {
        if (!fGroupFlag[gr]) // This group is marked as no data
            continue;
        for (int ch = 0; ch < 9; ch++)
        {
            if (!fFileManager[gr][ch]) // This channel has no file manager
            {
                continue;
            }
            // cout << "Reading: " << "Group: " << gr << " Channel: " << ch << endl;
            // fFileManager[gr][ch] -> PrintFileStatus(cout);
            // cout << "Test: " << endl;
            // cout <<  fSingleEventArray -> ConstructedAt(9*gr + ch) << endl;

            int temp = ReadOneEvent(*fFileManager[gr][ch], fHeader, *GetGlobalSingle(gr, ch));
            
            
            if(temp < 0)
            {
                return false;
            }
        }
    }
    return true;
}

void ProcessManager::ClearFileManager()
{
    if(fFileManagerInitiated)
    {
        for(int gr = 0; gr < 4; gr++)
        {
            for(int ch = 0; ch < 9; ch++)
            {
                delete fFileManager[gr][ch];
                fFileManager[gr][ch] = NULL;
            }
        }
    }
    fFileManagerInitiated = 0;
}

bool ProcessManager::FileInitiateByList()
{
    if(!fWriteFlag)
    {
        cerr << "Error! This Process cannot write into file, reading raw data initialization failed, please check configuration" << endl;
        return false;
    }
    if(fFileManagerInitiated)
    {
        cerr << "FileManager has already initiated in ProcessManager, if want to reinitiate, clear ProcessManager first." << endl;
        return true;
    }
    gSystem -> Exec("ls TR* wave* > .listfile");

    ifstream fin(".listfile");

    string sFileName;
    for(;fin.is_open() && fin.good();)
    {
        fin >> sFileName;

        int gr = 0, ch = 0;
        bool binflag = 0;
        bool test = OutputFileManager::ParseDataFileName(sFileName, gr, ch, binflag);
        if(test == false)
        {
            continue;
        }
        if(binflag != fBinFlag)
        {
            continue;
        }

        auto tFileManager = new OutputFileManager(sFileName);
        if(tFileManager -> IsValid() == false)
        {
            delete tFileManager;
            tFileManager = NULL;
        }
        else
        {
            fFileManager[gr][ch] = tFileManager;
            if(!fGroupFlag[gr])
            {
                fGroupFlag[gr] = 1;
            }
        }

        
    }    

    fFileManagerInitiated = 1;
    
    gSystem -> Exec("rm .listfile");
    return true;
}

bool ProcessManager::gInitiatedStatic = 0;
RootEvent_t *ProcessManager::gRootEvent = 0;
RootGroup_t *ProcessManager::gRootGroup[4]{0};
RootSingle_t *ProcessManager::gSingleEvent[4][9]{0};

void ProcessManager::InitiateStatic()
{
    if(gInitiatedStatic)
    {
        return;
    }
    gRootEvent = new RootEvent_t;
    for(int gr = 0; gr < 4; gr++)
    {
        gRootGroup[gr] = new RootGroup_t;
        for(int ch = 0; ch < 9; ch++)
        {
            gSingleEvent[gr][ch] = new RootSingle_t;
        }
    }
    gInitiatedStatic = 1;
}

RootEvent_t *&ProcessManager::GetGlobalEvent()
{
    if(!gInitiatedStatic)
    {
        InitiateStatic();
    }
    return gRootEvent;
}

RootGroup_t *&ProcessManager::GetGlobalGroup(int gr)
{
    if(!gInitiatedStatic)
    {
        InitiateStatic();
    }
    return gRootGroup[gr];
}

RootSingle_t *&ProcessManager::GetGlobalSingle(int gr, int ch)
{
    if(!gInitiatedStatic)
    {
        InitiateStatic();
    }
    return gSingleEvent[gr][ch];
}

