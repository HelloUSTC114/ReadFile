#ifndef READOUTPUTFILE_H
#define READOUTPUTFILE_H 1

#include <fstream>
#include <sstream>
#include "ParseConfigFile.h"

#include "DataType.h"

void FillZeroHeader(Header_t &header);

void FillZeroEvent(SingleEvent_t &event);
void FillZeroEvent(float *data);

class OutputFileManager
{
public:
    OutputFileManager() = default;
    ~OutputFileManager();
    OutputFileManager(string sfile);    // sfile is name of data file, group, channel information can be inferred from sfile, header information can be inferred from contents of file
    OutputFileManager(string sfile, int gr, int ch);    // sfile is name of configuration file, group and channel information must be provided, header and format information can be inferred from the information
    OutputFileManager(const OUTFILE_FLAGS &, int gr, int ch);
    
    bool OpenFile(string s);
    int ReadOneEvent(Header_t &, SingleEvent_t &);
    int ReadOneEvent(Header_t &, float *);

    bool IsValid(){return fStream.good() && fStream.is_open() && fFileNameParsed && fFileHeaderParsed;}

    void Reset(string sfile);
    void Reset(string sfile, int gr, int ch);
    void Clear();

    ostream& PrintFileStatus(ostream &os);

    static bool ParseDataFileName(string sfile, int &gr, int &ch, bool & binflag); // Only in charge of parse file name

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
    
    bool fFileAttributes = 0;   // if Attributes is assigned by given original configuration file, than channel and gruop info must be given meanwhile, and fFileNameParsed/fHeaderParsed should be set true;
    bool GenerateFileNameFromGroup(int gr, int ch);
    void ConvertFileFlag(const OUTFILE_FLAGS&);
};




#endif