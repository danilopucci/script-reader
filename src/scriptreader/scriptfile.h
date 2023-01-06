#ifndef SCRIPTFILE_H
#define SCRIPTFILE_H

#include "streambuffer.h"
#include <fstream>
#include <string>

class ScriptFile : public StreamBuffer
{
public:
    ScriptFile();
    ~ScriptFile();

    bool open(const std::string& fileNamePath);
    bool close();

    int getChar();
    void ungetChar();

    int pushLineCount();
    int addLineCount(int count);
    int popLineCount();
    int resetLineCount();
    int getLineCount();

    std::string getFileName();
    std::string getFilePath();
private:
    void setFileName();
    void setFilePath();

    std::string fileName;
    std::string fileNamePath;
    std::string filePath;
    std::fstream* file;
    int lineCount;

};

#endif // SCRIPTFILE_H