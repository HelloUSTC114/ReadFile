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
        fSingleEventArray = new TClonesArray("RootEvent_t", 40);
    }

    if(fFile -> IsOpen())
    {
        fFileName = filename;
        fWriteFlag = !ReadFlag;
        fReadFlag = ReadFlag;

        fRootHeader = new RootHeader_t;
        fRootEvent = new RootEvent_t;

        for(int i = 0; i < 4; i++)
        {
            fRootGroup[i] = new RootGroup_t;
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

    if(fSingleEventArray)
    {
        fSingleEventArray -> Clear();
        // fSingleEventArray -> Delete();   // Cannot use this delete function, because this function won't release this memory
        delete fSingleEventArray;
        fSingleEventArray = NULL;
    }

    delete fRootHeader;
    delete fRootEvent;
    fRootEvent = NULL;
    fRootHeader = NULL;

    for(int i = 0; i < 4; i++)
    {
        delete fRootGroup[i];
        fRootGroup[i] = NULL;
        fGroupFlag[i] = 0;
    }

    fBranch = NULL;

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
        fBranch = fTree -> Branch("EventBranch", &fRootEvent);
    }
    else if(fReadFlag)
    {
        fBranch = fTree -> GetBranch("EventBranch");
        fBranch -> SetAddress(&fRootEvent);
    }
    return true;

}

bool ProcessManager::ProcessOneEvent()
{
    if(!fWriteFlag)
        return false;
    ReadOneRaw();
    FillOneRaw();
}

bool ProcessManager::FillOneRaw()
{
    for(int gr = 0; gr < 4; gr ++)
    {
        if(!fGroupFlag[gr])     // This group is marked as no data
            continue;
        for(int ch = 0; ch < 9; ch++)
        {
            if(!fFileManager[gr][ch])   // This channel has no file manager
                continue;
            
            fRootGroup[gr] -> AddEvent(*(RootSingle_t*)fSingleEventArray->ConstructedAt(gr * 9 + ch), ch);
            
        }
        fRootEvent -> AddGroup(*fRootGroup[gr], gr);
    }
    fTree -> Fill();
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
                continue;
            ReadOneEvent(*fFileManager[gr][ch], fHeader, *(RootSingle_t *)fSingleEventArray->ConstructedAt(9 * gr + ch));
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
