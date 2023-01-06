#include "readscriptfile.h"
#include <string.h>
#include <stdio.h>
#include <cctype>
#include <iostream>

#include "scriptfile.h"


ReadScriptFile::ReadScriptFile()
{
    this->RecursionDepth = -1;

    this->Bytes.reserve(4);
    this->String.reserve(4000);
}

ReadScriptFile::~ReadScriptFile()
{
  if ( this->RecursionDepth != -1 )
  {
    this->printError("TReadScriptFile::~TReadScriptFile: Datei ist noch offen.\n");
    this->closeAll();
  }
}

void ReadScriptFile::readSymbol(char Symbol)
{
  this->nextToken();
  if ( this->Token != TOKEN_SPECIAL )
    this->error("special-char expected");

  if ( Symbol != this->Special )
    this->error("special-char expected");
}

std::string ReadScriptFile::readString()
{
  this->nextToken();
  return this->getString();
}

std::string ReadScriptFile::getString()
{
  if ( this->Token != TOKEN_STRING )
    this->error("string expected");

  return this->String;
}

char ReadScriptFile::readSpecial()
{
  this->nextToken();
  return this->getSpecial();
}

uint8_t ReadScriptFile::getSpecial()
{
  if ( this->Token != TOKEN_SPECIAL )
    this->error("special-char expected");

  return this->Special;
}

int ReadScriptFile::readNumber()
{
  int signal = 1;

  this->nextToken();

  if ( this->Token == TOKEN_SPECIAL && this->Special == '-' )
  {
    signal = -1;
    this->nextToken();
  }

  return this->getNumber() * signal;
}

int ReadScriptFile::getNumber()
{
  if ( this->Token != TOKEN_NUMBER )
    this->error("number expected");
  return this->Number;
}

std::string ReadScriptFile::readIdentifier(void)
{
  this->nextToken();
  return this->getIdentifier();
}

std::string ReadScriptFile::getIdentifier()
{
  if ( this->Token != TOKEN_IDENTIFIER )
    this->error("identifier expected");

  return strLower(this->String);
}

void ReadScriptFile::readCoordinate(int &x,int &y,int &z)
{    
  this->nextToken();
  this->getCoordinate(x, y, z);
}

void ReadScriptFile::getCoordinate(int &x,int &y,int &z)
{
  if ( this->Token != TOKEN_COORDINATE )
    this->error("coordinates expected");
  x = this->CoordX;
  y = this->CoordY;
  z = this->CoordZ;
}

uint8_t* ReadScriptFile::readBytesequence(void)
{
  this->nextToken();
  return this->getBytesequence();
}

uint8_t* ReadScriptFile::getBytesequence()
{
  if ( this->Token != TOKEN_BYTES )
    this->error("byte-sequence expected");
  return this->Bytes.data();
}

void ReadScriptFile::open(const std::string& name)
{
  std::string fileName = name;
  this->RecursionDepth += 1;
  if ( this->RecursionDepth >= MAX_RECURSION_DEPTH )
  {
    this->printError("TReadScriptFile::open: Rekursionstiefe zu gro");
    this->error("Recursion depth too high");
  }

  if ( this->RecursionDepth > 0 || fileName.front() != '/')
  {
      fileName = this->scriptFile->getFilePath() + fileName;
  }

  if(!this->Files[this->RecursionDepth].open(fileName))
  {
    this->printError("TReadScriptFile::open: Kann Datei " + fileName + " nicht ");
    this->error(fileName);

    --this->RecursionDepth;

    this->error("Cannot open script-file");
  }

  this->scriptFile = &this->Files[this->RecursionDepth];
}

bool ReadScriptFile::retrieveFilename(std::string &filename)
{
    bool result = false;
    int count = 0;

    if(this->scriptFile->getChar() != '@'){
        result = false;
        return result;
    }

    ScriptTokenPtr token = ScriptTokenPtr(new TokenString(*this->scriptFile));
    if(token->retrieve(filename, count)){
        result = true;
        return result;
    }

    return result;
}


bool ReadScriptFile::skipSpace()
{
    int c = 0;

    if(!std::isspace(this->scriptFile->getChar())){
        return false;
    }

    do{
        c = this->scriptFile->getChar();
    }while(std::isspace(c));

    this->scriptFile->ungetChar();

    return true;
}

bool ReadScriptFile::skipLine()
{
    int c = 0;

    if(this->scriptFile->getChar() != '#'){
        return false;
    }

    do{
        c = this->scriptFile->getChar();
    }while(c != '\n');

    this->scriptFile->pushLineCount();

    return true;
}

void ReadScriptFile::nextToken()
{

  if ( this->RecursionDepth == -1 )
  {
    this->printError("TReadScriptFile::nextToken: Kein Skript zum Lesen ge");
    this->Token = TOKEN_ENDOFFILE;
    return;
  }

  int c = 0;
  ScriptTokenPtr token;

  this->Number = 0;

  this->Bytes.clear();
  this->String.clear();

  try {
      while (true)
      {
          c = this->scriptFile->nextChar();

          if ( c == -1 ){
              if ( this->RecursionDepth <= 0 ){
                  this->Token = TOKEN_ENDOFFILE;
                  return;
              }
              this->internalClose();
              this->nextToken();
              return;
          }

          // skip and push line count
          if ( c == '\n' )
          {
              this->scriptFile->getChar();
              this->scriptFile->pushLineCount();
              continue;
          }

          // skip spaces
          if(std::isspace(c)){
              this->skipSpace();
              continue;
          }

          // skip comment
          if ( c == '#' )
          {
              this->skipLine();
              continue;
          }

          // retrieve and open file
          if ( c == '@' )
          {
              std::string filename;
              if(this->retrieveFilename(filename)){
                  this->open(filename);
                  this->nextToken();
              }
              return;
          }

          // retrieve identifier
          if ( std::isalpha(c) )
          {
              token = ScriptTokenPtr(new TokenIdentifier(*this->scriptFile));
              if(token->retrieve(this->String)){
                  this->Token = token->type;
                  return;
              }
          }

          // retrieve Number or ByteSequence
          if ( std::isdigit(c) )
          {
              token = ScriptTokenPtr(new TokenGenericNumber(*this->scriptFile));
              if(token->retrieve(this->Number, this->Bytes)){
                  this->Token = token->type;
                  return;
              }
          }

          // retrieve string
          if(c == '"')
          {
              int count = 0;
              token = ScriptTokenPtr(new TokenString(*this->scriptFile));
              if(token->retrieve(this->String, count)){
                  this->scriptFile->addLineCount(count);
                  this->Token = token->type;
                  return;
              }
          }

          {
              // If reaches here, it is special
              token = ScriptTokenPtr(new TokenSpecial(*this->scriptFile));
              if(token->retrieve(this->Special)){
                  this->Token = token->type;
                  return;
              }
          }

          {
              // If reaches here, can only be coordinate
              token = ScriptTokenPtr(new TokenCoordinate(*this->scriptFile));
              if(token->retrieve(this->CoordX, this->CoordY, this->CoordZ)){
                  this->Token = token->type;
                  return;
              }
          }

      }

  }  catch (const std::exception& e) {
     this->error(e.what());
  }

  this->printError("TReadScriptFile::nextToken: Ung");
}

void ReadScriptFile::error(const std::string &err)
{
  snprintf(this->ErrorString, 0x64u, "error in script-file \"%s\", line %d: %s", this->scriptFile->getFileName().c_str(), this->scriptFile->getLineCount(), err.c_str());

  this->closeAll();

  throw std::logic_error(this->ErrorString);
}

void ReadScriptFile::printError(const std::string &err)
{
    std::cout << err << std::endl;
}

void ReadScriptFile::internalClose(int fileIndex)
{
    int index = fileIndex ? fileIndex : this->RecursionDepth;

    if(!this->Files[index].close()){
        this->printError("TReadScriptFile::close: Fehler %d beim Schlie");
    }

    --this->RecursionDepth;
    this->scriptFile = &this->Files[this->RecursionDepth];
}

void ReadScriptFile::close()
{
  if ( this->RecursionDepth == -1 )
  {
    this->printError("TReadScriptFile::~TReadScriptFile: Datei ist noch offen.");
  }
  else
  {
    this->internalClose(this->RecursionDepth);
  }
}

void ReadScriptFile::closeAll()
{
    int recursion = this->RecursionDepth;
    for (int i = 0; i < recursion; i++ ){
      this->internalClose();
    }
}

std::string strLower(std::string a1)
{
    std::string lowerStr = a1;

    for(auto it = lowerStr.begin(); it != lowerStr.end(); it++){
        *it = std::tolower(*it);
    }

    return lowerStr;
}
