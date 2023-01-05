#include "scriptfile.h"

ScriptFile::ScriptFile()
{
    this->lineCount = 1;
}

ScriptFile::ScriptFile(const std::string& fileNamePath)
{
    this->lineCount = 1;
    this->fileNamePath = fileNamePath;

    file = new std::fstream(this->fileNamePath, std::ios_base::in | std::ios_base::binary);

    this->setFileName();
}

ScriptFile::~ScriptFile()
{
    this->lineCount = 1;
    this->fileNamePath = "";
    this->fileName = "";
    delete file;
}

bool ScriptFile::open()
{
    if(this->file != nullptr)
    {
        this->file = new std::fstream(this->fileNamePath, std::ios_base::in | std::ios_base::binary);
        return true;
    }
    return false;
}

bool ScriptFile::close()
{
    this->file->close();
    return true;
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
