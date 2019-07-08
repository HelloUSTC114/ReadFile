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

int ReadOutputFiles()
{

}