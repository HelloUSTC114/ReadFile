#define DEBUG
#include "FillRoot.h"
#include "ReadOneEvent.h"
#include "TFile.h"
#include "TTree.h"

#include "ProcessManager.h"
// Test FillRoot.h
// int main()
// {
//     RootEvent_t tEvent;
//     RootGroup_t tGroup;
//     RootSingle_t tSingle;

//     for(int i = 0; i < 1024; i++)
//     {
//         tSingle[i] = i;
//     }

//     for(int i = 0; i < 9; i++)
//     {
//         // cout << i << endl;
//         tGroup.AddEvent(tSingle, i);
//     }

//     for(int i = 0; i < 4; i++)
//     {
//         tEvent.SetGroupFlag(i, 1);
//     }
//     for (int i = 0; i < 4; i++)
//     {
//         tEvent.AddGroup(tGroup, i);
//     }



//     // // Verify TObject Write functions well
//     // auto file = new TFile("tEvent.root", "recreate");
//     // tEvent.Write("Event");
//     // cout << "Before Writing: " << endl;
//     // tEvent.PrintEvent(cout, 0);
//     // file ->Close();

//     // delete file;
//     // file = NULL;

//     // auto file2 = new TFile("tEvent.root");
//     // auto b = (RootEvent_t*)file2 -> Get("Event");

//     // cout << "Reading: " << endl;
    
//     // tEvent.PrintEvent(cout, 0);
//     // file2 -> Close();
//     // delete file2;
//     // file2 = NULL;

//     // Verify Tree 

//     auto treeFile = new TFile("tEventTree.root", "recreate");
//     auto eventTree = new TTree("Event", "Test Tree");
//     auto branch = eventTree -> Branch("EventBranch", &tEvent);

//     tEvent.PrintEvent(cout);
//     for(int i = 0; i < 32; i++)
//     {
//         eventTree -> Fill();
//     }
//     treeFile -> Write();
//     treeFile -> Close();

//     delete treeFile;
//     treeFile = NULL;

//     auto treeFile2 = new TFile("tEventTree.root");
//     auto eventTree2 = (TTree*)treeFile2 -> Get("Event");
//     cout << "Event Tree: " << eventTree2 << endl;
//     auto tTestEvent = new RootEvent_t;
//     cout << "Event tree entries: " << eventTree2 -> GetEntries() << endl;
//     eventTree2 -> SetBranchAddress("EventBranch", &tTestEvent);
//     for(int i = 0; i < 32; i++)
//     {
//         cout << "Entry: " << i << endl;
//         eventTree2 -> GetEntry(i);
//         tTestEvent -> PrintEvent(cout, 0);
//     }

//     treeFile2 -> Close();
//     delete treeFile2;
//     treeFile2 = NULL;
// }

// Test ReadOneEvent
// int main(int argc, char ** argv)
// {
//     // string filename = "WaveDumpConfig_X742.txt";
//     // OutputFileManager fileManager(filename, 2, 8);
//     OutputFileManager fileManager;
//     if(argc == 2)
//     {
//         string filename(argv[1]);
//         fileManager.Reset(filename);
//     }
//     else if(argc == 4)
//     {
//         string filename(argv[1]);
//         string sGr(argv[2]);
//         string sCh(argv[3]);
//         int gr = stoi(sGr);
//         int ch = stoi(sCh);
//         fileManager.Reset(filename, gr, ch);
//     }
//     fileManager.PrintFileStatus(cout);
//     RootSingle_t tSingle;
//     Header_t tHeader;
//     if(fileManager.IsValid())
//     {
//         fileManager.ReadOneEvent(tHeader, &tSingle[0]);
//         tSingle.Show(cout);
//     }
//     return 0;
// }

// Test ProcessManager

int main()
{
    ProcessManager a("TestProcess.root", 0);
}