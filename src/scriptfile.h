#ifndef SCRIPTFILE_H
#define SCRIPTFILE_H

#include "streambuffer.h"
#include <fstream>
#include <string>

class ScriptFile : public StreamBuffer
{
public:
    ScriptFile();
    ScriptFile(const std::string& fileNamePath);
    ~ScriptFile();

    bool open();
    bool close();

    int getChar();
    void ungetChar();

    int pushLineCount();
    int addLineCount(int count);
    int popLineCount();
    int resetLineCount();
    int getLineCount();

    std::string getFileName();
private:
    void setFileName();

    std::string fileName;
    std::string fileNamePath;
    std::fstream* file;
    int lineCount;

};

#endif // SCRIPTFILE_H
