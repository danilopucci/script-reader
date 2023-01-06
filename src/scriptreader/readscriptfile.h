#ifndef READSCRIPTFILE_H
#define READSCRIPTFILE_H

#include <cstdint>
#include <string>
#include <vector>
#include <fstream>

#include "scriptfile.h"
#include "scripttoken.h"

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

    void open(const std::string &fileNameStr);
    void close();

    void nextToken();

    void error(const std::string &Text);
    void printError(const std::string &err);

    void readSymbol(char);
    std::string readString();
    char readSpecial();
    int readNumber();
    std::string readIdentifier();
    void readCoordinate(int &x, int &y, int &z);
    std::vector<uint8_t> readBytesequence();

    std::string getString();
    uint8_t getSpecial();
    int getNumber();
    std::string getIdentifier();
    void getCoordinate(int &x, int &y, int &z);
    std::vector<uint8_t> getBytesequence();

private:

    bool retrieveFilename(std::string &filename);
    bool retrieveTokens();

    bool skipChars();
    bool skipLine();
    bool skipSpace();

    void closeAll();
    void internalClose(int fileIndex = 0);

public:
    ScriptTokenType Token;

private:
    static const int MAX_RECURSION_DEPTH = 3;

    ScriptFile Files[MAX_RECURSION_DEPTH];
    ScriptFile *scriptFile;

    char ErrorString[100];

    int RecursionDepth;

    char Special;
    std::vector<uint8_t> Bytes;
    std::string String;
    int Number;
    int CoordX;
    int CoordY;
    int CoordZ;
};

#endif // READSCRIPTFILE_H
