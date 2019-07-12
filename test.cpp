#include "FillRoot.h"
#include "TFile.h"
#include "TTree.h"
int main()
{
    RootEvent_t tEvent;
    RootSingle_t tSingle;

    tSingle.fData[0]=20;

    for(int i = 0; i < 3; i++)
    {
        cout << i << endl;
        tEvent.AddEvent(tSingle, i/8, i%8);
    }

    // // Verify TObject Write functions well
    // auto file = new TFile("tEvent.root", "recreate");
    // tEvent.Write("Event");
    // cout << "Before Writing: " << endl;
    // tEvent.PrintEvent(cout);
    // file ->Close();

    // delete file;
    // file = NULL;

    // auto file2 = new TFile("tEvent.root");
    // auto b = (RootEvent_t*)file2 -> Get("Event");

    // cout << "Reading: " << endl;
    
    // tEvent.PrintEvent(cout);
    // file2 -> Close();
    // delete file2;
    // file2 = NULL;

    // Verify Tree 

    auto treeFile = new TFile("tEventTree.root", "recreate");
    auto eventTree = new TTree("Event", "Test Tree");
    auto branch = eventTree -> Branch("EventBranch", &tEvent);

    tEvent.PrintEvent(cout);
    for(int i = 0; i < 32; i++)
    {
        eventTree -> Fill();
    }
    treeFile -> Write();
    treeFile -> Close();

    delete treeFile;
    treeFile = NULL;

    auto treeFile2 = new TFile("tEventTree.root");
    auto eventTree2 = (TTree*)treeFile2 -> Get("Event");
    cout << "Event Tree: " << eventTree2 << endl;
    auto tTestEvent = new RootEvent_t;
    cout << "Event tree entries: " << eventTree2 -> GetEntries() << endl;
    eventTree2 -> SetBranchAddress("EventBranch", &tTestEvent);
    for(int i = 0; i < 1; i++)
    {
        eventTree2 -> GetEntry(i);
        tTestEvent -> PrintEvent(cout);
    }

    treeFile2 -> Close();
    delete treeFile2;
    treeFile2 = NULL;
}