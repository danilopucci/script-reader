/****************************************************************************
 *
 *  Copyright (c) 2023, Danilo Pucci
 *
 *  This file is part of the MMORPG Script Reader project.
 *
 *  Source Code:
 *  https://github.com/danilopucci/script-reader/
 *
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files
 *  (the "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so,subject to the
 *  following conditions:
 *
 *  + The above copyright notice and this permission notice shall be included
 *    in all copies or substantial portions of the Software.
 *  + Credit is appreciated, but not required, if you find this project
 *    useful enough to include in your application, product, device, etc.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 *  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ***************************************************************************/

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

    char ErrorString[256];

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
