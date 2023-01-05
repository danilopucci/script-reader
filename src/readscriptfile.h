#ifndef READSCRIPTFILE_H
#define READSCRIPTFILE_H

#include <cstdint>
#include <string>
#include <vector>
#include <fstream>

#include "scriptfile.h"

std::string strLower(std::string a1);

enum TOKEN{
    ENDOFFILE        = 0,
    IDENTIFIER       = 1,
    NUMBER           = 2,
    STRING           = 3,
    BYTES            = 4,
    COORDINATE       = 5,
    SPECIAL          = 6
};

class ReadScriptFile
{
public:
    ReadScriptFile();

    ~ReadScriptFile();

    void readSymbol(char);
    std::string readString();
    char readSpecial();
    int readNumber();
    std::string readIdentifier(void);
    void readCoordinate(int &x, int &y, int &z);
    uint8_t *readBytesequence(void);

    void nextToken();

    std::string getString();
    uint8_t getSpecial();
    int getNumber();
    std::string getIdentifier();
    void getCoordinate(int &x, int &y, int &z);
    uint8_t *getBytesequence();

    void open(const std::string &fileNameStr);
    void close();
    void closeAll();

    void error(const std::string &Text);

private:
    void setToken(TOKEN token);

    bool retrieveIdentifier();
    bool retrieveNumber();
    bool retrieveNumberOrBytes();
    bool retrieveCoordinate();

    bool retrieveString();

    void internalClose(int fileIndex = 0);

    void skipLine();
    void skipSpace();
public:
//private:
    TOKEN Token;
    char Special;
    int RecursionDepth;
    std::vector<uint8_t> Bytes;
    std::string String;
    int Number;
    int CoordX;
    int CoordY;
    int CoordZ;

    std::string Filename[3];
    char ErrorString[100];

    ScriptFile* Files[3];
    ScriptFile *scriptFile;
};



#endif // READSCRIPTFILE_H
