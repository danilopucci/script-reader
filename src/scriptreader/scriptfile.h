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
