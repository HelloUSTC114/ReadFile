#include "FillRoot.h"
#include "TFile.h"
int main()
{
    RootEvent_t tEvent;
    RootSingle_t tSingle;

    tSingle.fData[0]=20;

    for(int i = 0; i < 32; i++)
    {
        cout << i << endl;
        tEvent.AddEvent(tSingle, i/8, i%8);
    }

    auto file = new TFile("tEvent.root", "recreate");
    tEvent.Write("Event");
    cout << "Before Writing: " << endl;
    tEvent.PrintEvent(cout);
    file ->Close();

    delete file;

    auto file2 = new TFile("tEvent.root");
    auto b = (RootEvent_t*)file2 -> Get("Event");

    cout << "Reading: " << endl;
    
    tEvent.PrintEvent(cout);
    file2 -> Close();
}