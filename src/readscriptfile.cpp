#include "readscriptfile.h"
#include <string.h>
#include <stdio.h>
#include <cctype>

#include "scriptfile.h"
#include "scripttoken.h"

ReadScriptFile::ReadScriptFile()
{
    this->RecursionDepth = -1;

    this->Bytes.reserve(4);
    this->String.reserve(4000);

    this->Sign = 1;
}

ReadScriptFile::~ReadScriptFile()
{
  if ( this->RecursionDepth != -1 )
  {
    error("TReadScriptFile::~TReadScriptFile: Datei ist noch offen.\n");
    this->closeAll();
  }
}

int ReadScriptFile::getChar()
{
    this->lastGottenCharHistorical[1] = this->lastGottenCharHistorical[0];
    //this->lastGottenChar = this->Files[this->RecursionDepth]->get();
    this->lastGottenChar = this->scriptFile->getChar();
    this->lastGottenCharHistorical[0] = this->lastGottenChar;
    return this->lastGottenChar;
}

void ReadScriptFile::ungetChar(int c)
{
    this->lastGottenCharHistorical[0] = this->lastGottenCharHistorical[1];
    this->lastGottenChar = this->lastGottenCharHistorical[1];
    //this->Files[this->RecursionDepth]->unget();
    this->scriptFile->ungetChar();
}

void ReadScriptFile::readSymbol(char Symbol)
{
  this->nextToken();
  if ( this->Token != SPECIAL )
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
  if ( this->Token != STRING )
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
  if ( this->Token != SPECIAL )
    this->error("special-char expected");

  return this->Special;
}

int ReadScriptFile::readNumber()
{
  int signal = 1;

  this->nextToken();

  if ( this->Token == SPECIAL && this->Special == '-' )
  {
    signal = -1;
    this->nextToken();
  }

  return this->getNumber() * signal;
}

int ReadScriptFile::getNumber()
{
  if ( this->Token != NUMBER )
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
  if ( this->Token != IDENTIFIER )
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
  if ( this->Token != COORDINATE )
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
  if ( this->Token != BYTES )
    this->error("byte-sequence expected");
  return this->Bytes.data();
}

void ReadScriptFile::open(const std::string& FileName)
{
  size_t index; // eax

  this->RecursionDepth += 1;
  if ( this->RecursionDepth == 3 )
  {
    this->error("TReadScriptFile::open: Rekursionstiefe zu gro");
    this->error("Recursion depth too high");
  }
LABEL_9:
  if ( this->RecursionDepth <= 0 || FileName.front() == '/'

    || ( this->Filename[this->RecursionDepth] = std::string(this->Filename[this->RecursionDepth-1]),
        (index = findLast(this->Filename[this->RecursionDepth], '/')) == 0) )
  {
    this->Filename[this->RecursionDepth] = FileName;
  }
  else
  {
      this->Filename[this->RecursionDepth] = this->Filename[this->RecursionDepth].substr(0, index + 1).append(FileName);
  }

  //this->Files[this->RecursionDepth] = new std::fstream(this->Filename[this->RecursionDepth], std::ios_base::in | std::ios_base::binary);
  this->scriptFile = this->Files[this->RecursionDepth] = new ScriptFile(this->Filename[this->RecursionDepth]);

  if(!this->scriptFile)
  //if ( !this->Files[this->RecursionDepth] )
  {
    this->error("TReadScriptFile::open: Rekursionstiefe zu gro");
    this->error(this->Filename[this->RecursionDepth]);

    --this->RecursionDepth;

    this->error("Cannot open script-file");
    goto LABEL_9;
  }
  //this->Line[this->RecursionDepth] = 1;
}

bool ReadScriptFile::retrieveIdentifier()
{
    bool result = true;

    ScriptToken *token = new TokenIdentifier(*this->scriptFile);
    token->retrieveIdentifier(this->String);
    delete token;

    this->setToken(IDENTIFIER);
    return result;
}

bool ReadScriptFile::retrieveNumberOrBytes()
{
    this->Bytes.clear();
    this->retrieveNumber();
    this->Bytes.emplace_back(this->Number);

    while(this->getChar() == '-'){
        this->retrieveNumber();
        this->Bytes.emplace_back(this->Number);
        this->Token = BYTES;
    }

    this->ungetChar(this->lastGottenChar);
    return false;
}

bool ReadScriptFile::retrieveCoordinate()
{
    ScriptToken *token = new TokenCoordinate(*this->scriptFile);

    token->retrieveCoordinate(this->CoordX, this->CoordY, this->CoordZ);
    delete token;

    this->setToken(COORDINATE);
    return true;
}

bool ReadScriptFile::retrieveNumber()
{
    this->Number = 0;

    int v6 = 0;
    bool result = true;

    while(true){

        v6 = this->getChar();

        if ( v6 == -1 ){
            result = false;
            break;
        }

        if ( std::isdigit(v6) ){
            this->Number = v6 + 10 * this->Number - '0';
        }else{
            this->ungetChar(v6);
            result = false;
            break;
        }
    }

    this->Token = NUMBER;
    return result;
}

bool ReadScriptFile::retrieveString()
{
    int c = 0;
    bool result = false;
    ScriptToken *token = new TokenString(*this->scriptFile);
    //int count = token->retrieveString(this->String);
    int count = token->retrieve(this->String);
    delete token;

    if(count > 0){
        this->scriptFile->addLineCount(count);

    }


    this->setToken(STRING);
    return result;
}


void ReadScriptFile::skipSpace()
{
    int c;
    do{
        c = this->getChar();
    }while(std::isspace(c));

    this->ungetChar(c);
}

void ReadScriptFile::skipLine()
{
    int c;
    do{
        c = this->getChar();
    }while(c != '\n');

    this->scriptFile->pushLineCount();
}

void ReadScriptFile::nextToken()
{

  if ( this->RecursionDepth == -1 )
  {
    this->error("TReadScriptFile::nextToken: Kein Skript zum Lesen ge");
    this->Token = ENDOFFILE;
    return;
  }

  int v6 = 0;

  this->Number = 0;
  this->Sign = 1;

  this->Bytes.clear();
  this->String.clear();

  while (true)
  {

      v6 = this->getChar();

      if ( v6 == -1 ){
          if ( this->RecursionDepth <= 0 ){
              this->setToken(ENDOFFILE);
              return;
          }
          this->internalClose();
          this->nextToken();
          return;
      }

      if ( v6 == '\n' )
      {
          this->scriptFile->pushLineCount();
          continue;
      }

      if(std::isspace(v6)){
          this->ungetChar(v6);
          this->skipSpace();
          continue;
      }

      if ( v6 == '#' )
      {
          this->skipLine();
          continue;
      }

      if ( v6 == '@' )
      {
          this->retrieveString();
          this->open(this->String);
          this->nextToken();
          return;
      }

      if ( std::isalpha(v6) )
      {
          this->ungetChar(v6);
          this->retrieveIdentifier();
          return;

      }

      if ( std::isdigit(v6) )
      {
          this->ungetChar(v6);
          this->retrieveNumberOrBytes();
          return;
      }

      if(v6 == '"')
      {
          this->ungetChar(v6);
          this->retrieveString();
          return;
      }

      if(v6 == '[' || v6 == '<' || v6 == '>' || v6 == '-'){
          this->ungetChar(v6);
          ScriptToken *token = new TokenSpecial(*this->scriptFile);
          if(token->retrieveSpecial(this->Special) >= 0){
              this->Token = SPECIAL;
              delete token;
              return;
          }

          this->retrieveCoordinate();
          delete token;
          return;
      }

      this->Special = v6;
      this->setToken(SPECIAL);
      return;


  }
}

void ReadScriptFile::setToken(TOKEN token)
{
    this->Token = token;
}

void ReadScriptFile::error(const std::string &Text)
{
  size_t index; // eax
  const char *v3; // ecx

  index = findLast(this->Filename[this->RecursionDepth], '/');
  if ( index ){
    v3 = &this->Filename[this->RecursionDepth].at(index + 1);
  }else{
    v3 = this->Filename[this->RecursionDepth].c_str();
  }

  snprintf(this->ErrorString, 0x64u, "error in script-file \"%s\", line %d: %s", v3, this->scriptFile->getLineCount(), Text.c_str());

  this->closeAll();
}

void ReadScriptFile::internalClose(int fileIndex)
{
    int index = fileIndex ? fileIndex : this->RecursionDepth;
    this->Files[index]->resetLineCount();
    this->Files[index]->close();

    --this->RecursionDepth;
    this->scriptFile = this->Files[this->RecursionDepth];
}

void ReadScriptFile::close()
{
  if ( this->RecursionDepth == -1 )
  {
    this->error("TReadScriptFile::close: Keine Datei offen.\n");
  }
  else
  {
    this->internalClose(this->RecursionDepth);
  }
}

void ReadScriptFile::closeAll()
{
    int recursion = this->RecursionDepth;
    for (int i = 0; i < recursion; i++ )
    {
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

size_t findLast(const std::string& str, char c)
{
  size_t index = 0;

  index = str.find_last_of(c);

  if(index == std::string::npos){
      index = 0;
  }

  return index;
}
