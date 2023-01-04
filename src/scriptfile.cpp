#include "scriptfile.h"

ScriptFile::ScriptFile()
{
    this->lineCount = 0;
}

ScriptFile::ScriptFile(const std::string& filename)
{
    this->lineCount = 0;
    this->filename = filename;

    file = new std::fstream(this->filename, std::ios_base::in | std::ios_base::binary);
}

ScriptFile::~ScriptFile()
{
    this->lineCount = 0;
    this->filename = "";
    delete file;
}

bool ScriptFile::open()
{
    if(this->file != nullptr)
    {
        this->file = new std::fstream(this->filename, std::ios_base::in | std::ios_base::binary);
        return true;
    }
    return false;
}

bool ScriptFile::close()
{
    this->close();
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
