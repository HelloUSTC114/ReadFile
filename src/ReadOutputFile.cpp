#include "ReadOutputFile.h"

void OutputFileManager::ConvertFileFlag(OUTFILE_FLAGS fileflag)
{
    // Check the file format type
    if(fileflag & OFF_BINARY)
    {
        // Binary file format
        fBinaryFlag = 1;
    }
    else 
    {
        // ASCII file format
        fBinaryFlag = 0;
    }

    if(fileflag & OFF_HEADER)
    {
        // Has header
        fHeaderFlag = 1;
    }
    else
    {
        // No header
        fHeaderFlag = 0;
    }
    
}

OutputFileManager::OutputFileManager(string sfile)
{
    ParseDataFileName(sfile);
    ParseDataFileHeader(sfile);
}

int OutputFileManager::ReadOneEvent(EventInfo_t & info, SingleEvent_t &event)
{
    
}

bool OutputFileManager::ParseDataFileName(string sfile)
{
    if (sfile.find(".txt") != string::npos)
    {
        fFileName = sfile;
        fBinaryFlag = 0;
        fStream.open(fFileName);
    }
    else if (sfile.find(".dat") != string::npos)
    {
        fFileName = sfile;
        fBinaryFlag = 1;
        fStream.open(fFileName, ifstream::binary);
    }
    else
    {
        cerr << "Error! Cannot parse file format! File: " << sfile << endl;
        return false;
    }

    if (fFileName.find("TR") != string::npos)
    {
        fTRFlag = 1;
        string sub_temp = fFileName.substr(3, 1); // Judge channel
        stringstream ss_temp(sub_temp);
        int ch = 0;
        ss_temp >> ch;
        fChannel = ch;
    }
    else if (fFileName.find("wave") != string::npos)
    {
        fTRFlag = 0;
        string sub_temp = fFileName.substr(5, 2); // Judge channel
        stringstream ss_temp(sub_temp);
        int ch = 0;
        ss_temp >> ch;
        fChannel = ch;
    }
    else
    {
        cerr << "Error! Cannot parse channel! File: " << fFileName << endl;
        return false;
    }
    fFileNameParsed = 1;
    return true;
}

bool OutputFileManager::ParseDataFileHeader(string sfile)
{
    if(!fFileNameParsed)
    {
        bool temp = ParseDataFileName(sfile);
        if(!temp)
        {
            cerr << "Error! Cannot parse header before parse filename!" << endl;
            return false;
        }
    }
    ifstream tStream;
    if(fBinaryFlag) // Binary format
    {
        tStream.open(fFileName, fstream::binary);
        uint32_t BinHeader[6];
        tStream.read((char*)BinHeader, sizeof(*BinHeader) * 6);
        if(BinHeader[0] == 1024)
        {
            fHeaderFlag = 1;
        }
        else
        {
            fHeaderFlag = 0;
        }
        tStream . close();
        
    }
    else    // ASCII format
    {
        tStream.open(fFileName);
        string tString;
        tStream >> tString;
        try
        {
            stof(tString);
            fHeaderFlag = 0;
        }
        catch(invalid_argument)
        {
            if(tString == "Record")
                fHeaderFlag = 1;
            else
            {
                cerr << "Wrong Header format! File: " << fFileName << endl;
                return false;
            }
            
        }

        tStream.close();
    }
    return true;
}

bool OutputFileManager::OpenFile()
{
    if(fStream.good() == false)
    {
        fStream . close();
    }
    if(fStream.is_open() == false)
    {
        fStream.open(fFileName);
        if(fStream.is_open() == false)
        {
            return false;
        }
    }
    return true;
}