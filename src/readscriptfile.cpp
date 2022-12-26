#include "readscriptfile.h"
#include <string.h>
#include <stdio.h>

ReadScriptFile::ReadScriptFile()
{
    this->RecursionDepth = -1;
    this->Bytes = reinterpret_cast<uint8_t*>(this->String);
}

ReadScriptFile::~ReadScriptFile()
{
  if ( this->RecursionDepth != -1 )
  {
    error("TReadScriptFile::~TReadScriptFile: Datei ist noch offen.\n");
    for (int i = this->RecursionDepth; i != -1; i = this->RecursionDepth )
    {
      if ( fclose(this->File[i]) )
      {
        this->error("TReadScriptFile::close: Fehler %d beim Schlie");
      }
      --this->RecursionDepth;
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

void ReadScriptFile::nextToken()
{
  int v1; // esi
  char *v2; // edi
  unsigned int v3; // eax
  int v4; // eax
  int v5; // eax
  int v6; // edi
  int v7; // eax
  int *v8; // eax
  int v9; // eax
  int v10; // eax
  int v11; // edi
  int v12; // eax
  int v13; // edi
  int v14; // eax
  int v15; // edi
  int v16; // eax
  int v17; // eax
  int v18; // eax
  int v19; // eax
  int v20; // edi
  int v21; // eax
  int v22; // eax
  int v23; // edi
  int v24; // eax
  int v25; // eax
  int v26; // edi
  int v27; // eax
  int v28; // eax
  int v29; // eax
  int v30; // eax
  int v31; // eax
  int v32; // eax
  char *v33; // [esp+1Ch] [ebp-1Ch]
  int Sign; // [esp+20h] [ebp-18h]
  FILE *f; // [esp+24h] [ebp-14h]
  int pos; // [esp+28h] [ebp-10h]

  if ( this->RecursionDepth == -1 )
  {
    this->error("TReadScriptFile::nextToken: Kein Skript zum Lesen ge");
    this->Token = ENDOFFILE;
    return;
  }
  v33 = this->String;
LABEL_3:
  pos = 0;
  v1 = 0;
  v2 = this->String;
  v3 = 4000;
  if ( ((uint64_t)v33 & 4) != 0 )
  {
    v2 = &this->String[4];
    v3 = 3996;
    *this->String = 0;
  }
  memset(v2, 0, 4 * (v3 >> 2));
  v4 = this->RecursionDepth;
  this->Number = 0;
  Sign = 1;
  f = this->File[v4];
  while ( 2 )
  {
    if ( pos == 3999 )
      this->error("string too long");
    switch ( v1 )
    {
      case 0:
        v5 = getc(f);
        v6 = v5;
        if ( v5 == -1 ){
            if ( this->RecursionDepth <= 0 ){
                this->setToken(ENDOFFILE);
                return;
            }
            if ( fclose(this->File[this->RecursionDepth]) )
            {
                this->error("TReadScriptFile::close: Fehler %d beim Schlie");
            }
            --this->RecursionDepth;
            goto LABEL_3;
        }
        if ( v5 == 10 )
        {
          ++this->Line[this->RecursionDepth];
        }
        else if ( !std::isspace(v5) )
        {

          if ( v6 == '#' )
          {
            v1 = 1;
          }else if ( v6 == '@' )
          {
            v1 = 30;
          }else if ( std::isalpha(v6) )
          {
            v1 = 2;
            this->String[pos++] = v6;

          }else if ( std::isdigit(v6) )
          {
              this->Number = v6 - '0';
              v1 = 3;
          }else if(v6 == '"'){
                  v1 = 6;

          }else if(v6 == '['){
              v1 = 11;
          }else if ( v6 == '<' )
          {
              v1 = 22;
          }else if ( v6 == '>' )
          {
              v1 = 25;
          }else if ( v6 == '-' )
          {
              v1 = 27;
          }else{
              this->Special = v6;
              this->setToken(SPECIAL);
              return;
          }
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
        if ( fclose(this->File[this->RecursionDepth]) )
        {
          this->error("TReadScriptFile::close: Fehler %d beim Schlie");
        }
        --this->RecursionDepth;
        goto LABEL_3;
      case 2:
        v10 = getc(f);
        v11 = v10;
        if ( pos == 30 )
          this->error("identifier too long");
        if ( v10 == -1 ){
            this->setToken(IDENTIFIER);
            return;
        }

        if ( std::isalpha(v10) || std::isdigit(v11) || v11 == '_' ){
            this->String[pos++] = v11;
            continue;
        }

        ungetc(v11, f);
        this->setToken(IDENTIFIER);
        return;
      case 3:
        v12 = getc(f);
        v13 = v12;
        if ( v12 == -1 ){
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
        return;
      case 4:
        v14 = getc(f);
        v15 = v14;
        if ( v14 == -1 )
          this->error("unexpected end of file");
        if ( !std::isdigit(v14) )
          this->error("syntax error");
        v1 = 5;
        this->Number = v15 - 48;
        continue;
      case 5:
        v16 = getc(f);
        v13 = v16;
        if ( v16 == -1 ){
            this->Bytes[pos] = this->Number;
            this->Token = BYTES;
            return;
        }

        if ( std::isdigit(v16) )
        {
          this->Number = v13 + 10 * this->Number - 48;
        }
        else
        {
          if ( v13 != 45 )
          {
            ungetc(v13, f);

            this->Bytes[pos] = this->Number;
            this->Token = BYTES;
            return;
          }

          this->Bytes[pos++] = this->Number;
          v1 = 4;
        }
        continue;
      case 6:
        v17 = getc(f);
        v11 = v17;
        if ( v17 == -1 )
          this->error("unexpected end of file");
        if ( v17 == '"' )
        {
          this->setToken(STRING);
          return;
        }
        else if ( v17 == '\\' )
        {
          v1 = 7;
        }
        else
        {
          if ( v17 == 10 )
            ++this->Line[this->RecursionDepth];
          this->String[pos++] = v11;
        }
        continue;
      case 7:
        v18 = getc(f);
        if ( v18 == -1 )
          this->error("unexpected end of file");
        if ( v18 == 110 )
          this->String[pos] = 10;
        else
          this->String[pos] = v18;
        ++pos;
        v1 = 6;
        continue;

      case 11:
        v19 = getc(f);
        this->Special = '[';
        v20 = v19;
        if ( v19 == -1 ){
            this->Token = SPECIAL;
            return;
        }
        if ( std::isdigit(v19) )
        {
          Sign = 1;
          this->Number = v20 - 48;
          v1 = 12;
          continue;
        }
        if ( v20 == 45 )
        {
          Sign = -1;
          this->Number = 0;
          v1 = 12;
          continue;
        }

        ungetc(v20, f);
        this->Token = SPECIAL;
        return;
      case 12:
        v21 = getc(f);
        v13 = v21;
        if ( v21 == -1 )
          this->error("unexpected end of file");
        if ( std::isdigit(v21) ){
            this->Number = v13 + 10 * this->Number - 48;
            continue;
        }

        if ( v13 != 44 )
          this->error("syntax error");
        v1 = 13;
        this->CoordX = this->Number * Sign;
        continue;
      case 13:
        v22 = getc(f);
        v23 = v22;
        if ( v22 == -1 )
          this->error("unexpected end of file");
        if ( std::isdigit(v22) )
        {
          Sign = 1;
          this->Number = v23 - 48;
        }
        else
        {
          if ( v23 != 45 )
            this->error("syntax error");
          Sign = -1;
          this->Number = 0;
        }
        v1 = 14;
        continue;
      case 14:
        v24 = getc(f);
        v13 = v24;
        if ( v24 == -1 )
          this->error("unexpected end of file");
        if ( std::isdigit(v24) ){
            this->Number = v13 + 10 * this->Number - 48;
            continue;
        }

        if ( v13 != 44 )
          this->error("syntax error");
        v1 = 15;
        this->CoordY = this->Number * Sign;
        continue;
      case 15:
        v25 = getc(f);
        v26 = v25;
        if ( v25 == -1 )
          this->error("unexpected end of file");
        if ( std::isdigit(v25) )
        {
          Sign = 1;
          this->Number = v26 - 48;
        }
        else
        {
          if ( v26 != 45 )
            this->error("syntax error");
          Sign = -1;
          this->Number = 0;
        }
        v1 = 16;
        continue;
      case 16:
        v27 = getc(f);
        v13 = v27;
        if ( v27 == -1 )
          this->error("unexpected end of file");
        if ( std::isdigit(v27) ){
            this->Number = v13 + 10 * this->Number - 48;
            continue;
        }

        if ( v13 != 93 )
          this->error("syntax error");

        this->CoordZ = this->Number * Sign;
        this->setToken(COORDINATE);
        return;

      case 22:
        v28 = getc(f);
        this->Special = '<';
        v20 = v28;
        if ( v28 == -1 ){
            this->Token = SPECIAL;
            return;
        }

        if ( v28 != '=' )
        {
          if ( v28 != '>' ){
              ungetc(v20, f);
              this->Token = SPECIAL;
              return;
          }
          this->Special = 'N';
          this->setToken(SPECIAL);
          return;
        }else{
            this->Special = 'L';
            this->setToken(SPECIAL);
            return;
        }
        continue;

      case 25:
        v29 = getc(f);
        this->Special = '>';;
        v20 = v29;
        if ( v29 == -1 ){
            this->Token = SPECIAL;
            return;
        }

        if ( v29 != 61 ){
            ungetc(v20, f);
            this->Token = SPECIAL;
            return;
        }
        this->Special = 'G';
        this->setToken(SPECIAL);
        return;

      case 27:
        v30 = getc(f);
        this->Special = '-';;
        if ( v30 == -1 ){
            this->Token = SPECIAL;
            return;
        }

        if ( v30 == '>' ){
            this->Special = 'I';
            this->setToken(SPECIAL);
            return;
        }

        ungetc(v30, f);
        this->Token = SPECIAL;
        return;


      case 30:
        v31 = getc(f);
        if ( v31 == -1 )
          this->error("unexpected end of file");
        if ( v31 != 34 )
          this->error("syntax error");
        v1 = 31;
        continue;

      case 31:
        v32 = getc(f);
        v11 = v32;
        if ( v32 == -1 )
          this->error("unexpected end of file");
        if ( v32 != 34 ){
            this->String[pos++] = v11;
        }else{
            v1 = 32;
        }
        continue;

      case 32:
        this->open( v33);
        goto LABEL_3;

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
    if ( fclose(this->File[v1]) )
    {
      error("TReadScriptFile::close: Fehler %d beim Schlie");
    }
    --this->RecursionDepth;
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
