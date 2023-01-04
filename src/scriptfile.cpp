#include "scriptfile.h"

ScriptFile::ScriptFile()
{
    this->fileLine = 0;
}

ScriptFile::ScriptFile(const std::string& filename)
{
    this->fileLine = 0;
    this->filename = filename;

    file = new std::fstream(this->filename, std::ios_base::in | std::ios_base::binary);
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

int ScriptFile::pushLine()
{
    return this->fileLine++;
}

int ScriptFile::addLine(int count)
{
    return this->fileLine += count;
}

int ScriptFile::popLine()
{
    return this->fileLine--;
}
