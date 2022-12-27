#include "readscriptfile.h"
#include <string.h>
#include <stdio.h>

ReadScriptFile::ReadScriptFile()
{
    this->RecursionDepth = -1;
    this->Bytes = reinterpret_cast<uint8_t*>(this->String);

    this->pos = 0;
    this->Sign = 1;
}

ReadScriptFile::~ReadScriptFile()
{
  if ( this->RecursionDepth != -1 )
  {
    error("TReadScriptFile::~TReadScriptFile: Datei ist noch offen.\n");
    for (int i = this->RecursionDepth; i != -1; i = this->RecursionDepth )
    {
        this->internalClose(i);
    }
  }
}


void ReadScriptFile::readSymbol(char Symbol)
{
  this->nextToken();
  if ( this->Token != SPECIAL )
    this->error("special-char expected");
  if ( Symbol != this->Special )
    this->error("special-char expected");
}

char* ReadScriptFile::readString()
{
  this->nextToken();
  if ( this->Token != STRING )
    this->error("string expected");
  return this->String;
}

char ReadScriptFile::readSpecial()
{
  this->nextToken();
  if ( this->Token != SPECIAL )
    this->error("special-char expected");
  return this->Special;
}

int ReadScriptFile::readNumber()
{
  TOKEN v1; // edx
  int v2; // esi

  this->nextToken();
  v1 = this->Token;
  v2 = 1;
  if ( this->Token == SPECIAL && this->Special == '-' )
  {
    v2 = -1;
    this->nextToken();
    v1 = this->Token;
  }
  if ( v1 != NUMBER )
    this->error("number expected");
  if ( this->Token != NUMBER )
    this->error("number expected");
  return this->Number * v2;
}

char* ReadScriptFile::readIdentifier(void)
{
  this->nextToken();
  if ( this->Token != IDENTIFIER )
    this->error("identifier expected");
  strLower(this->String);
  return this->String;
}

void ReadScriptFile::readCoordinate(int &x,int &y,int &z)
{
  this->nextToken();
  if ( this->Token != COORDINATE )
    this->error("coordinates expected");
  x = this->CoordX;
  y = this->CoordY;
  z = this->CoordZ;
}

uint8_t* ReadScriptFile::readBytesequence(void)
{
  this->nextToken();
  if ( this->Token != BYTES )
    this->error("byte-sequence expected");
  return this->Bytes;
}

void ReadScriptFile::open(char *FileName)
{
  char *v3; // eax
  int v4; // edi
  FILE *v5; // eax
  int v6; // ebx
  int v7; // edi
  char *v8; // eax

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
  v4 = this->RecursionDepth;
  v5 = fopen(this->Filename[v4], "rb");
  v6 = this->RecursionDepth;
  this->File[v4] = v5;
  if ( !this->File[v6] )
  {
    this->error("TReadScriptFile::open: Rekursionstiefe zu gro");
    this->error(this->Filename[v6]);

    --this->RecursionDepth;

    this->error("Cannot open script-file");
    goto LABEL_9;
  }
  this->Line[v6] = 1;
}

bool ReadScriptFile::retrieveIdentifier(FILE* f)
{
    int tmp = 0;
    int c = 0;
    c = getc(f);
    tmp = c;

    if ( pos == 30 )
      this->error("identifier too long");
    if ( c == -1 ){
        this->setToken(IDENTIFIER);
        return false;
    }

    if ( std::isalpha(tmp) || std::isdigit(tmp) || tmp == '_' ){
        this->String[pos++] = tmp;
        return true;
    }

    ungetc(tmp, f);
    this->setToken(IDENTIFIER);
    return false;
}

bool ReadScriptFile::retrieveNumber(FILE* f)
{
    /*int tmp = 0;
    int c = 0;
    c = getc(f);
    v13 = c;
    if ( c == -1 ){
        this->Token = NUMBER;
        return;
    }
    if ( std::isdigit(v12) ){
        this->Number = v13 + 10 * this->Number - 48;
        continue;
    }

    if ( v13 == 45 ){
        this->Bytes[pos++] = this->Number;
        v1 = 4;
        continue;
    }
    ungetc(v13, f);

    this->Token = NUMBER;
    return;*/
}

bool ReadScriptFile::retrieveCoordinate(FILE* f)
{
    this->retrieveCoordinateSign(f);
    while(this->retrieveCoordinateByAxis(f, this->CoordX)) {}

    this->retrieveCoordinateSign(f);
    while(this->retrieveCoordinateByAxis(f, this->CoordY)) {}

    this->retrieveCoordinateSign(f);
    while(this->retrieveCoordinateByAxis(f, this->CoordZ)) {}

    this->setToken(COORDINATE);
    return true;
}

bool ReadScriptFile::retrieveCoordinateSign(FILE* f)
{
    int tmp = this->getNextChar(f);

    if ( std::isdigit(tmp) )
    {
      Sign = 1;
      this->Number = tmp - '0';
    }
    else
    {
      if ( tmp != '-' )
        this->error("syntax error");

      Sign = -1;
      this->Number = 0;
    }
    return true;
}

bool ReadScriptFile::retrieveCoordinateByAxis(FILE* f, int &value)
{
    int tmp = this->getNextChar(f);

    if ( std::isdigit(tmp) ){
        this->Number = tmp + 10 * this->Number - '0';
        return true;
    }

    if ( tmp != ',' && tmp != ']')
      this->error("syntax error");


    value = this->Number * Sign;
    return false;
}

bool ReadScriptFile::retrieveRelationalOperator(FILE* f)
{
    int c = 0;
    if ( this->Special == '<' )
    {
        if(!this->getNextSpecial(f, c)){
            return false;
        }

        if ( c == '=' )
        {
            this->Special = 'L';
            this->setToken(SPECIAL);
            return false;

        }else{
            if ( c != '>' ){
                ungetc(c, f);
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
        if(!this->getNextSpecial(f, c)){
            return false;
        }

        if ( c == '=' ){
            this->Special = 'G';
            this->setToken(SPECIAL);
            return false;
        }
        else{
            ungetc(c, f);
            this->Token = SPECIAL;
            return false;
        }
    }
    return false;
}

bool ReadScriptFile::retrieveSeparator(FILE* f)
{
    int c = 0;
    if(!this->getNextSpecial(f, c)){
        return false;
    }

    if ( c == '>' ){
        this->Special = 'I';
        this->setToken(SPECIAL);
        return true;
    }

    ungetc(c, f);
    this->Token = SPECIAL;
    return false;
}

bool ReadScriptFile::retrieveString(FILE* f)
{
    int c = this->getNextChar(f);

    if ( c == '"' )
    {
        this->setToken(STRING);
        return false;
    }
    else if ( c == '\\' )
    {
        c = this->getNextChar(f);

        if ( c == 110 )
            this->String[pos] = 10;
        else
            this->String[pos] = c;

        ++pos;
    }
    else
    {
        if ( c == 10 )
            ++this->Line[this->RecursionDepth];
        this->String[pos++] = c;
    }
    return true;

}

bool ReadScriptFile::retrieveFilename(FILE* f)
{
    int c = this->getNextChar(f);

    if ( c != '"' )
        this->error("syntax error");


    do{
        c = this->getNextChar(f);

        if ( c != '"' ){
            this->String[pos++] = c;
        }
    }while(c != '"');
    return true;
}

int ReadScriptFile::getNextChar(FILE* f)
{
    int c = getc(f);

    if ( c == -1 )
      this->error("unexpected end of file");

    return c;
}

bool ReadScriptFile::getNextSpecial(FILE *f, int &c)
{
    c = getc(f);

    if ( c == -1 ){
        this->Token = SPECIAL;
        return false;
    }

    return true;
}

void ReadScriptFile::nextToken()
{
  int v6; // edi
  int v7; // eax
  int v9; // eax
  int v10; // eax

  int v12; // eax

  int v14; // eax

  int v16; // eax
  int v17; // eax
  int v18; // eax
  int v19; // eax

  int v28; // eax
  int v29; // eax
  int v30; // eax
  int v31; // eax

  FILE *f; // [esp+24h] [ebp-14h]

  if ( this->RecursionDepth == -1 )
  {
    this->error("TReadScriptFile::nextToken: Kein Skript zum Lesen ge");
    this->Token = ENDOFFILE;
    return;
  }
LABEL_3:

  int v1 = 0;
  char * v2 = this->String;
  unsigned int  v3 = 4000;

  if ( ((uint64_t)this->String & 4) != 0 )
  {
    v2 = &this->String[4];
    v3 = 3996;
    *this->String = 0;
  }
  memset(v2, 0, 4 * (v3 >> 2));

  this->Number = 0;
  this->Sign = 1;
  this->pos = 0;

  f = this->File[this->RecursionDepth];
  while ( 2 )
  {
    if ( pos == 3999 )
      this->error("string too long");
    switch ( v1 )
    {
      case 0:
        v6 = getc(f);
        if ( v6 == -1 ){
            if ( this->RecursionDepth <= 0 ){
                this->setToken(ENDOFFILE);
                return;
            }
            this->internalClose();
            goto LABEL_3;
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
            v1 = 1;
        }else if ( v6 == '@' )
        {
            this->retrieveFilename(f);
            this->open( this->String);
            goto LABEL_3;
        }else if ( std::isalpha(v6) )
        {
            v1 = 2;
            this->String[pos++] = v6;

        }else if ( std::isdigit(v6) )
        {
            this->Number = v6 - '0';
            v1 = 3;
        }else if(v6 == '"'){
            while(this->retrieveString(f)){}
            return;

        }else if(v6 == '['){

            this->Special = '[';
            v18 = getc(f);

            if ( v18 == -1 ){
                this->Token = SPECIAL;
                return;
            }

            ungetc(v18, f);
            if(!std::isdigit(v18) && v18 != '-'){

                this->Token = SPECIAL;
                return;
            }

            this->retrieveCoordinate(f);
            return;

        }else if ( v6 == '<' || v6 == '>')
        {
            this->Special = v6;
            if(!this->retrieveRelationalOperator(f)){
                return;
            }

        }else if ( v6 == '-' )
        {
            this->Special = '-';
            this->retrieveSeparator(f);
            return;
        }else{
            this->Special = v6;
            this->setToken(SPECIAL);
            return;
        }

        continue;
      case 1:
        v9 = getc(f);
        if ( v9 != -1 )
        {
          if ( v9 == 10 )
          {
            ++this->Line[this->RecursionDepth];
            v1 = 0;
          }
          continue;
        }

        if ( this->RecursionDepth <= 0 ){
            this->setToken(ENDOFFILE);
            return;
        }
        this->internalClose();
        goto LABEL_3;
      case 2:

        if(!this->retrieveIdentifier(f)){
            return;
        }
        continue;

      case 3:
        v12 = getc(f);

        if ( v12 == -1 ){
            this->Token = NUMBER;
            return;
        }
        if ( std::isdigit(v12) ){
            this->Number = v12 + 10 * this->Number - '0';
            continue;
        }

        if ( v12 == '-' ){
            this->Bytes[pos++] = this->Number;
            v1 = 4;
            continue;
        }
        ungetc(v12, f);

        this->Token = NUMBER;
        return;
      case 4:
        v14 = this->getNextChar(f);

        if ( !std::isdigit(v14) )
          this->error("syntax error");


        this->Number = v14 - 48;

        v1 = 5;
        continue;
      case 5:
        v16 = getc(f);

        if ( v16 == -1 ){
            this->Bytes[pos] = this->Number;
            this->Token = BYTES;
            return;
        }

        if ( std::isdigit(v16) )
        {
          this->Number = v16 + 10 * this->Number - 48;
        }
        else
        {
          if ( v16 != 45 )
          {
            ungetc(v16, f);

            this->Bytes[pos] = this->Number;
            this->Token = BYTES;
            return;
          }

          this->Bytes[pos++] = this->Number;
          v1 = 4;
        }
        continue;

//      case 32:
//        this->open( this->String);
//        goto LABEL_3;

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

char* ReadScriptFile::getString()
{
  if ( this->Token != STRING )
    this->error("string expected");
  return this->String;
}

uint8_t ReadScriptFile::getSpecial()
{
  if ( this->Token != SPECIAL )
    this->error("special-char expected");
  return this->Special;
}

int ReadScriptFile::getNumber()
{
  if ( this->Token != NUMBER )
    this->error("number expected");
  return this->Number;
}

char* ReadScriptFile::getIdentifier()
{
  if ( this->Token != IDENTIFIER )
    this->error("identifier expected");
  strLower(this->String);
  return this->String;
}

void ReadScriptFile::getCoordinate(int &x,int &y,int &z)
{
  if ( this->Token != COORDINATE )
    this->error("coordinates expected");
  x = this->CoordX;
  y = this->CoordY;
  z = this->CoordZ;
}

uint8_t* ReadScriptFile::getBytesequence()
{
  if ( this->Token != BYTES )
    this->error("byte-sequence expected");
  return this->Bytes;
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
  for (int i = this->RecursionDepth; i != -1; i = this->RecursionDepth )
  {
    if ( fclose(this->File[i]) )
    {
      this->error("ErrorString");
    }
    --this->RecursionDepth;
  }
}

void ReadScriptFile::internalClose(int fileIndex)
{
    int index = fileIndex ? fileIndex : this->RecursionDepth;

    if ( fclose(this->File[index]) )
    {
      this->error("TReadScriptFile::close: Fehler %d beim Schlie");
    }
    --this->RecursionDepth;
}

void ReadScriptFile::close()
{
  int v1; // eax

  v1 = this->RecursionDepth;
  if ( v1 == -1 )
  {
    this->error("TReadScriptFile::close: Keine Datei offen.\n");
  }
  else
  {
    this->internalClose(v1);
  }
}

char * strLower(char *a1)
{
  int v1; // esi
  char i; // al
  char v3; // dl
  unsigned int v4; // eax
  char v5; // cl

  v1 = 0;
  for ( i = *a1; i; i = a1[v1] )
  {
    v3 = i;
    v4 = i - 65;
    if ( v4 <= 0x19 || (v5 = v3, (uint8_t)(v4 - 127) <= 0x1Eu) )
      v5 = v3 + 32;
    a1[v1++] = v5;
  }
  return a1;
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
