#ifndef READOUTPUTFILE_H
#define READOUTPUTFILE_H 1

#include <fstream>
#include <sstream>
#include "ParseConfigFile.h"

#include "DataType.h"

void FillZeroHeader(Header_t &header);

void FillZeroEvent(SingleEvent_t &event);

class OutputFileManager
{
public:
    OutputFileManager(string sfile);    // sfile is name of data file, group, channel information can be inferred from sfile, header information can be inferred from contents of file
    OutputFileManager(string sfile, int gr, int ch);    // sfile is name of configuration file, group and channel information must be provided, header and format information can be inferred from the information
    void ConvertFileFlag(const OUTFILE_FLAGS&);
    bool OpenFile(string s);
    int ReadOneEvent(Header_t &, SingleEvent_t &);

    bool IsValid(){return fStream.good() && fStream.is_open();}
private:
    string fFileName;
    ifstream fStream;

    int fGroup;
    int fChannel;
    bool fTRFlag;
    bool fBinaryFlag;   // 0: ASCII, 1: Binary
    bool fHeaderFlag;   // 0: No header, 1: Header

    int fEventCounter = 0;  // Eventcounter

    bool fFileNameParsed = 0;
    bool fFileHeaderParsed = 0;
    bool ParseDataFileName(string sfile);   // Only in charge of parse file name
    bool ParseDataFileHeader(string sfile); // Only in charge of parse header, open file and then close file, using another stream

    void FillHeaderManually(Header_t &header);

    bool OpenFile();    // In charge of openfile
    
    bool fFileAttributes = 0;
    bool GenerateFileNameFromGroup(int gr, int ch);
};




#endif