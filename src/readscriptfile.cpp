#include "readscriptfile.h"
#include <string.h>
#include <stdio.h>
#include <cctype>
#include <limits>

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
    this->lastGottenChar = this->Files[this->RecursionDepth]->get();
    this->lastGottenCharHistorical[0] = this->lastGottenChar;
    return this->lastGottenChar;
}

void ReadScriptFile::ungetChar(int c)
{
    this->lastGottenCharHistorical[0] = this->lastGottenCharHistorical[1];
    this->lastGottenChar = this->lastGottenCharHistorical[1];
    this->Files[this->RecursionDepth]->unget();
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
  int v2; // esi

  this->nextToken();
  v2 = 1;
  if ( this->Token == SPECIAL && this->Special == '-' )
  {
    v2 = -1;
    this->nextToken();
  }

  return this->getNumber() * v2;
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
  this->readSymbol('[');

  this->CoordX = this->readNumber();
  this->readSymbol(',');

  this->CoordY = this->readNumber();
  this->readSymbol(',');

  this->CoordZ = this->readNumber();

  this->readSymbol(']');

  this->Token = COORDINATE;

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

void ReadScriptFile::open(const std::string& fileNameStr)
{
  const char *FileName = fileNameStr.c_str();
  char *v3; // eax

  this->RecursionDepth += 1;
  if ( this->RecursionDepth == 3 )
  {
    this->error("TReadScriptFile::open: Rekursionstiefe zu gro");
    this->error("Recursion depth too high");
  }
LABEL_9:
  if ( this->RecursionDepth <= 0 || *FileName == '/'

    || (strcpy(this->Filename[this->RecursionDepth], this->Filename[this->RecursionDepth-1]),
        (v3 = findLast(this->Filename[this->RecursionDepth], 47)) == 0) )
  {
    strcpy(this->Filename[this->RecursionDepth], FileName);
  }
  else
  {
    strcpy(v3 + 1, FileName);
  }

  this->Files[this->RecursionDepth] = new std::fstream(this->Filename[this->RecursionDepth], std::ios_base::in | std::ios_base::binary);

  if ( !this->Files[this->RecursionDepth] )
  {
    this->error("TReadScriptFile::open: Rekursionstiefe zu gro");
    this->error(this->Filename[this->RecursionDepth]);

    --this->RecursionDepth;

    this->error("Cannot open script-file");
    goto LABEL_9;
  }
  this->Line[this->RecursionDepth] = 1;
}

bool ReadScriptFile::retrieveIdentifier()
{
    int c = 0;
    bool result = true;

    if(std::isalpha(this->lastGottenChar)){
        this->String.push_back(this->lastGottenChar);
    }

    while(true){
        c = this->getChar();

        if (this->String.length() == 30 )
            this->error("identifier too long");

        if ( c == -1 ){
            result = false;
            break;
        }

        if ( std::isalpha(c) || std::isdigit(c) || c == '_' ){
            this->String.push_back(c);
        }else{
            this->ungetChar(c);
            result = false;
            break;
        }
    }

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

bool ReadScriptFile::retrieveRelationalOperator()
{
    int c = 0;
    if ( this->Special == '<' )
    {
        if(!this->getNextSpecial(c)){
            return false;
        }

        if ( c == '=' )
        {
            this->Special = 'L';
            this->setToken(SPECIAL);
            return false;

        }else{
            if ( c != '>' ){
                this->ungetChar(c);
                this->Token = SPECIAL;
                return false;
            }

            this->Special = 'N';
            this->setToken(SPECIAL);
            return false;
        }
    }
    else if ( this->Special == '>' )
    {
        if(!this->getNextSpecial(c)){
            return false;
        }

        if ( c == '=' ){
            this->Special = 'G';
            this->setToken(SPECIAL);
            return false;
        }
        else{
             this->ungetChar(c);
            this->Token = SPECIAL;
            return false;
        }
    }
    return false;
}

bool ReadScriptFile::retrieveSeparator()
{
    int c = 0;
    if(!this->getNextSpecial(c)){
        return false;
    }

    if ( c == '>' ){
        this->Special = 'I';
        this->setToken(SPECIAL);
        return true;
    }

    this->ungetChar(c);
    this->Token = SPECIAL;
    return false;
}

bool ReadScriptFile::retrieveString()
{
    int c = 0;
    bool result = true;

    c = this->getChar();

    if(c != '"'){
        return false;
    }

    while((c = this->getChar()) != '"'){
        if ( c == '\\' )
        {
            c = this->getNextChar();

            if ( c == 110 ){
                this->String.push_back(10);
            }else{
                this->String.push_back(c);
            }
        }
        else
        {
            if ( c == 10 )
                ++this->Line[this->RecursionDepth];

            this->String.push_back(c);
        }
    }

    this->setToken(STRING);
    return result;
}

int ReadScriptFile::getNextChar()
{
    int c = this->getChar();

    if ( c == -1 )
      this->error("unexpected end of file");

    return c;
}

bool ReadScriptFile::getNextSpecial(int &c)
{
    c = this->getChar();

    if ( c == -1 ){
        this->Token = SPECIAL;
        return false;
    }

    return true;
}

void ReadScriptFile::skipSpace()
{
    int c;
    do{
        c = this->getChar();
    }while(std::isspace(c));
}

void ReadScriptFile::skipLine()
{
    this->Files[this->RecursionDepth]->ignore(std::numeric_limits<std::streamsize>::max(), 10);
    ++this->Line[this->RecursionDepth];
}

void ReadScriptFile::nextToken()
{

  if ( this->RecursionDepth == -1 )
  {
    this->error("TReadScriptFile::nextToken: Kein Skript zum Lesen ge");
    this->Token = ENDOFFILE;
    return;
  }

  int v1 = 0;
  int v2 = 0;
  int v6 = 0;

  this->Number = 0;
  this->Sign = 1;

  this->Bytes.clear();
  this->String.clear();

  while ( 2 )
  {
    if ( this->String.length() == 3999 ){
      this->error("string too long");
    }

    switch ( v1 )
    {
      case 0:
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
        if ( v6 == 10 )
        {
          ++this->Line[this->RecursionDepth];
        }
        if(std::isspace(v6)){

            continue;
        }

        if ( v6 == '#' )
        {
            this->skipLine();
            v1 = 0;

            if(this->Files[this->RecursionDepth]->eof()){
                if ( this->RecursionDepth <= 0 ){
                    this->setToken(ENDOFFILE);
                    return;
                }
                this->internalClose();
                this->nextToken();
                return;
            }

        }else if ( v6 == '@' )
        {
            this->retrieveString();
            this->open(this->String);
            this->nextToken();
            return;
        }else if ( std::isalpha(v6) )
        {
            this->retrieveIdentifier();
            return;

        }else if ( std::isdigit(v6) )
        {
            this->ungetChar(v6);
            this->retrieveNumberOrBytes();
            return;
        }else if(v6 == '"'){
            this->ungetChar(v6);
            this->retrieveString();
            return;

        }else if ( v6 == '<' || v6 == '>')
        {
            this->Special = v6;
            if(!this->retrieveRelationalOperator()){
                return;
            }

        }else if ( v6 == '-' )
        {
            this->Special = '-';
            this->retrieveSeparator();
            return;
        }else{
            this->Special = v6;
            this->setToken(SPECIAL);
            return;
        }

        continue;

      default:
        this->error("TReadScriptFile::nextToken: Ung");
        v1 = 0;
        continue;
    }
  }
}

void ReadScriptFile::setToken(TOKEN token)
{
    this->Token = token;
}

void ReadScriptFile::error(const std::string &Text)
{
  char *v2; // eax
  const char *v3; // ecx

  v2 = findLast(this->Filename[this->RecursionDepth], 47);
  if ( v2 )
    v3 = v2 + 1;
  else
    v3 = this->Filename[this->RecursionDepth];

  snprintf(this->ErrorString, 0x64u, "error in script-file \"%s\", line %d: %s", v3, this->Line[this->RecursionDepth], Text.c_str());

 this->closeAll();
}

void ReadScriptFile::internalClose(int fileIndex)
{
    int index = fileIndex ? fileIndex : this->RecursionDepth;
    this->Files[index]->close();
    this->Line[index] = 0;
    --this->RecursionDepth;
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

char * findLast(char *s, char c)
{
  char *v2; // ebx
  char *v4; // eax

  v2 = 0;
  while ( 1 )
  {
    v4 = strchr(s, c);
    if ( !v4 )
      break;
    v2 = v4;
    s = v4 + 1;
  }
  return v2;
}
