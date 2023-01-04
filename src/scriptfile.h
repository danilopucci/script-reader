#ifndef SCRIPTFILE_H
#define SCRIPTFILE_H

#include "streambuffer.h"
#include <fstream>
#include <string>

class ScriptFile : public StreamBuffer
{
public:
    ScriptFile();
    ScriptFile(const std::string& filename);

    bool open();
    bool close();

    int getChar();
    void ungetChar();

    int pushLine();
    int addLine(int count);
    int popLine();

private:
    std::string filename;
    std::fstream* file;
    int fileLine;

};

#endif // SCRIPTFILE_H
