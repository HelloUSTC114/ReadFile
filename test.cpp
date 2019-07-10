#include "FillRoot.h"
#include "TFile.h"
int main()
{
    RootEvent_t test(10);
    auto file = new TFile("test.root", "recreate");
    test.Write();
    file ->Close();
    cout << "Before Writing: " << endl;
    for (int i = 0; i < test.fChannelNumber; i++)
    {
        cout << "channel: " << i << endl;
        for (int j = 0; j < 500; j++)
        {
            cout << test.fData[i].Data[j] << '\t';
        }
        cout << endl;
    }
    delete file;

    auto file2 = new TFile("test.root");
    auto b = (RootEvent_t*)file2 -> Get("RootEvent_t;1");

    cout << "Reading: " << endl;
    
    for(int i = 0; i < b->fChannelNumber; i++)
    {
        cout << "channel: " << i << endl;
        for (int j = 0; j < 500; j++)
        {
            cout << b->fData[i].Data[j] << '\t';
        }
        cout << endl;
    }
    file2 -> Close();
}