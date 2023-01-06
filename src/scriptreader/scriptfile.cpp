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
