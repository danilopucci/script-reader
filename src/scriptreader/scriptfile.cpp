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
#include "scriptfile.h"

ScriptFile::ScriptFile()
{
    this->file = new std::fstream();
    this->lineCount = 0;
}

ScriptFile::~ScriptFile()
{
    delete file;
}

bool ScriptFile::open(const std::string &fileNamePath)
{
    this->fileNamePath = fileNamePath;
    this->setFileName();
    this->setFilePath();
    this->lineCount = 1;

    if(this->file != nullptr)
    {
        this->file->open(this->fileNamePath, std::ios_base::in | std::ios_base::binary);
        return this->file->is_open();
    }
    return false;
}

bool ScriptFile::close()
{
    this->fileNamePath = "";
    this->fileName = "";
    this->lineCount = 0;

    this->file->close();
    return !this->file->is_open();
}

int ScriptFile::getChar()
{
    return this->file->get();
}

void ScriptFile::ungetChar()
{
    this->file->unget();
}

int ScriptFile::pushLineCount()
{
    return this->lineCount++;
}

int ScriptFile::addLineCount(int count)
{
    return this->lineCount += count;
}

int ScriptFile::popLineCount()
{
    return this->lineCount--;
}

int ScriptFile::resetLineCount()
{
    return this->lineCount = 0;
}

int ScriptFile::getLineCount()
{
    return this->lineCount;
}

void ScriptFile::setFileName()
{
    size_t index = this->fileNamePath.find_last_of('/');

    if(index != std::string::npos){
        this->fileName = this->fileNamePath.substr(index + 1);
    }else{
        this->fileName = this->fileNamePath;
    }
}

std::string ScriptFile::getFileName()
{
    return this->fileName;
}

void ScriptFile::setFilePath()
{
    size_t index = this->fileNamePath.find_last_of('/');

    if(index != std::string::npos){
        this->filePath = this->fileNamePath.substr(0, index + 1);
    }else{
        this->filePath = this->fileNamePath;
    }
}

std::string ScriptFile::getFilePath()
{
    return this->filePath;
}
