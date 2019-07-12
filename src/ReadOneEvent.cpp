#include "ReadOneEvent.h"

void OutputFileManager::ConvertFileFlag(const OUTFILE_FLAGS &fileflag)
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

    fFileAttributes = 1;
    
}

OutputFileManager::OutputFileManager(string sfile)
{
    ParseDataFileName(sfile);
    ParseDataFileHeader(sfile);

    OpenFile();
}

OutputFileManager::OutputFileManager(string sfile, int gr, int ch):
fGroup(gr), fChannel(ch)
{
    auto pFile = fopen(sfile.c_str(), "read");
    if(!pFile)
    {
        cerr << "Error! Cannot open configuration file! File name: " << sfile << endl;
        return;
    }
    WaveDumpConfig_t config;
    ParseConfigFile(pFile,&config);
    ConvertFileFlag(config.OutFileFlags);
    GenerateFileNameFromGroup(gr, ch);
    OpenFile();
}

void OutputFileManager::Clear()
{
    fFileName.clear();
    fStream . close();
    fStream.clear();
    fEventCounter = 0;
    fFileAttributes = 0;
    fFileNameParsed = 0;
    fFileHeaderParsed = 0;
}

void OutputFileManager::Reset(string sfile)
{
    Clear();
    ParseDataFileName(sfile);
    ParseDataFileHeader(sfile);
    OpenFile();
}

void OutputFileManager::Reset(string sfile, int gr, int ch)
{
    Clear();
    auto pFile = fopen(sfile.c_str(), "read");
    if (!pFile)
    {
        cerr << "Error! Cannot open configuration file! File name: " << sfile << endl;
        return;
    }
    WaveDumpConfig_t config;
    ParseConfigFile(pFile, &config);
    ConvertFileFlag(config.OutFileFlags);
    GenerateFileNameFromGroup(gr, ch);
    OpenFile();
}

OutputFileManager::~OutputFileManager()
{
    Clear();
}

void OutputFileManager::FillHeaderManually(Header_t& header)
{
    header.fLength = 1024;
    header.fBoardID = 0000;
    header.fPattern = 0;
    if (fTRFlag)
    {
        fChannel = 8;
    }
    else
    {
        header.fChannel = fChannel % 4;
    }

    header.fEventCounter = fEventCounter;
    header.fTriggerTimeTag = 0;
}

int OutputFileManager::ReadOneEvent(Header_t & header, SingleEvent_t &event)
{
    ReadOneEvent(header, event.Data);
}

int OutputFileManager::ReadOneEvent(Header_t & header, float* data)
{
    if (!fStream.good() || !fStream.is_open())
    {
        FillZeroHeader(header);
        FillZeroEvent(data);
        return -1;
    }

    fEventCounter++;
    if (fBinaryFlag) // binary file
    {
        if (fHeaderFlag) // Has header, read header from file
        {
            fStream.read((char *)&header.fLength, sizeof(uint32_t));         // Read fLength
            fStream.read((char *)&header.fBoardID, sizeof(uint32_t));        // Read fBoardID
            fStream.read((char *)&header.fPattern, sizeof(uint32_t));        // Read fPattern
            fStream.read((char *)&header.fChannel, sizeof(uint32_t));        // Read fChannel
            fStream.read((char *)&header.fEventCounter, sizeof(uint32_t));   // Read fEventCounter
            fStream.read((char *)&header.fTriggerTimeTag, sizeof(uint32_t)); // Read fTriggerTimeTag
        }
        else // No header, fill header manually
        {
            FillHeaderManually(header);
        }
        // Fill event into event array;
        fStream.read((char *)data, 1024 * 4);
    }
    else // ASCII file
    {
        if (fHeaderFlag) // Has header
        {
            string sLine;
            for (int i = 0; i < 7; i++)
            {
                getline(fStream, sLine);
                stringstream ss(sLine);
                string sWord;
                ss >> sWord;
                if (sWord == "Record")
                {
                    header.fLength = 1024;
                }
                else if (sWord == "BoardID:")
                {
                    ss >> sWord;
                    try
                    {
                        int temp = stoi(sWord);
                        header.fBoardID = temp;
                    }
                    catch (invalid_argument)
                    {
                        header.fBoardID = 0;
                    }
                }
                else if (sWord == "Channel:")
                {
                    ss >> sWord;
                    try
                    {
                        int ch = stoi(sWord);
                        header.fChannel = ch;
                    }
                    catch (invalid_argument)
                    {
                        header.fChannel = 8;
                    }
                }
                else if (sWord == "Event")
                {
                    ss >> sWord;
                    ss >> sWord;
                    try
                    {
                        int counter = stoi(sWord);
                        header.fEventCounter = counter;
                    }
                    catch (invalid_argument)
                    {
                        header.fEventCounter = 0;
                    }
                }
                else if (sWord == "Pattern:")
                {
                    ss >> sWord;
                    try
                    {
                        int pattern = stoi(sWord);
                        header.fPattern = pattern;
                    }
                    catch (invalid_argument)
                    {
                        header.fPattern = 8;
                    }
                }
                else if (sWord == "Trigger")
                {
                    ss >> sWord >> sWord;
                    ss >> sWord;
                    try
                    {
                        int time = stoi(sWord);
                        header.fTriggerTimeTag = time;
                    }
                    catch (invalid_argument)
                    {
                        header.fTriggerTimeTag = 0;
                    }
                }
            }
        }
        else
        {
            FillHeaderManually(header);
        }

        for (int i = 0; i < 1024; i++)
        {
            float temp;
            fStream >> temp;
            data[i] = temp;
        }
    }
    return 1;
}

ostream& OutputFileManager::PrintFileStatus(ostream &os)
{
    if(fStream.is_open())
    {
        os << "File is open, file name: " << fFileName << endl;
        if(fStream.good())
        {
            os << "File is good." << endl;
        }
    }
    else
    {
        os << "File name: " << fFileName << endl;
        os << "File has not open yet." << endl;
    }
    if(fFileAttributes)
    {
        os << "Reading attributes and file names has been parsed through original configuration file." << endl;
    }
    else if(fFileNameParsed)
    {
        os << "Binary/ASCII and Channel info is parsed through data file name." << endl;
    }
    else if(fFileHeaderParsed)
    {
        os << "Header info is parsed through attemped to read data." << endl;
    }
    if(fFileNameParsed)
    {
        os << "Group: " << fGroup << endl;
        os << "Channel: ";
        if(fTRFlag)
        {
            os << "TR" << endl;
        }
        else
        {
            os << fChannel << endl;
        }
        os << "Binary/ASCII: ";
        if(fBinaryFlag)
        {
            os << "Binary" << endl;
        }
        else
        {
            os << "ASCII" << endl;
        }
    }
    if(fFileHeaderParsed)
    {
        os << "Header: " << fHeaderFlag << endl;
    }
    if(IsValid())
    {
        os << "Everything is good, ready to read a new event." << endl;
    }
    else
    {
        os << "Something wrong, please check file name." << endl;
    }
    

    return os;


    
}

bool OutputFileManager::ParseDataFileName(string sfile)
{
    if (sfile.find(".txt") != string::npos)
    {
        fFileName = sfile;
        fBinaryFlag = 0;
    }
    else if (sfile.find(".dat") != string::npos)
    {
        fFileName = sfile;
        fBinaryFlag = 1;
    }
    else
    {
        cerr << "Error! Cannot parse file format! File: " << sfile << endl;
        return false;
    }

    if (fFileName.find("TR") != string::npos)
    {
        fTRFlag = 1;
        string sub_temp = fFileName.substr(3, 3); // Judge channel
        if(sub_temp == "0_0")
        {
            fGroup = 0;
        }
        else if(sub_temp == "0_1")
        {
            fGroup = 1;
        }
        else if(sub_temp == "0_2")
        {
            fGroup = 2;
        }
        else if(sub_temp == "1_3")
        {
            fGroup = 3;
        }
        fChannel = 8;
    }
    else if (fFileName.find("wave") != string::npos)
    {
        fTRFlag = 0;
        string sub_temp = fFileName.substr(5, 2); // Judge channel
        stringstream ss_temp(sub_temp);
        int ch = 0;
        ss_temp >> ch;
        fGroup = ch / 8;
        fChannel = ch % 8;
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
        if(BinHeader[3] == fChannel)
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
    fFileHeaderParsed = 1;
    return true;
}

bool OutputFileManager::OpenFile()
{
    if(fStream.good() == false)
    {
        fStream . close();
        fEventCounter = 0;
    }
    if(fStream.is_open() == false)
    {
        if(fBinaryFlag)
        {
            fStream.open(fFileName, ifstream::binary);
        }
        else
        {
            fStream.open(fFileName);
        }
        
        if(fStream.is_open() == false)
        {
            return false;
        }
    }
    return true;
}

bool OutputFileManager::GenerateFileNameFromGroup(int gr, int ch)
{
    if(!fFileAttributes)
    {
        cerr << "File attributions have not been decided yet!" << endl;
        return false;
    }
    fGroup = gr;
    fChannel = ch;
    if(ch == 8) // For TR
    {
        fFileName = "TR_";
        if(gr == 0 || gr == 1 || gr == 2)
        {
            fFileName += "0_";
            fFileName += to_string(gr);
        }
        else
        {
            fFileName += "1_3";
        }
    }
    else
    {
        fFileName = "wave_";
        fFileName += to_string(8 * gr + ch);
    }

    if(fBinaryFlag)
    {
        fFileName += ".dat";
    }
    else
    {
        fFileName += ".txt";
    }

    fFileNameParsed = 1;
    fFileHeaderParsed = 1;
    return true;
}

void FillZeroHeader(Header_t &header)
{
    header.fLength = 0;
    header.fBoardID = 0;
    header.fPattern = 0;
    header.fChannel = 0;
    header.fEventCounter = 0;
    header.fTriggerTimeTag = 0;
}

void FillZeroEvent(SingleEvent_t &event)
{
    for (int i = 0; i < 1024; i++)
    {
        event.Data[i] = 0;
    }
}

void FillZeroEvent(float *data)
{
    for(int i = 0;i < 1024; i++)
    {
        data[i] = 0;
    }
}