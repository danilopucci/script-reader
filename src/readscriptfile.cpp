#include "readscriptfile.h"
#include <string.h>
#include <stdio.h>
#include <cctype>

#include "scriptfile.h"
#include "scripttoken.h"

/*
talvez criar uma classe de stream com implementacoes de skip
fazer classe de File com funcoes basicas de open/close/getpath/line
verificar necessidade dos gets, reads da readScriptFile
organizar o acesso do recursion nos Files[]. Talvez criar funcao que acesse diretamente o atual
checar como era implementada as exceptions e recoloca-las
- verificar exceptions dos files
colocar em bloco try-catch

*/

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

  this->Files[this->RecursionDepth] = new ScriptFile(fileName);

  if(!this->Files[this->RecursionDepth]->open())
  {
    this->printError("TReadScriptFile::open: Kann Datei " + fileName + " nicht ");
    this->error(fileName);

    --this->RecursionDepth;

    this->error("Cannot open script-file");
  }

  this->scriptFile = this->Files[this->RecursionDepth];
}

bool ReadScriptFile::retrieveIdentifier()
{
    bool result = true;

    ScriptToken *token = new TokenIdentifier(*this->scriptFile);
    token->retrieve(this->String);
    delete token;

    this->setToken(IDENTIFIER);
    return result;
}

bool ReadScriptFile::retrieveNumberOrBytes()
{
    this->Bytes.clear();
    this->retrieveNumber();
    this->Bytes.emplace_back(this->Number);

    while(this->scriptFile->getChar() == '-'){
        this->retrieveNumber();
        this->Bytes.emplace_back(this->Number);
        this->Token = BYTES;
    }

    this->scriptFile->ungetChar();
    return false;
}

bool ReadScriptFile::retrieveCoordinate()
{
    ScriptToken *token = new TokenCoordinate(*this->scriptFile);

    token->retrieve(this->CoordX, this->CoordY, this->CoordZ);
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

        v6 = this->scriptFile->getChar();

        if ( v6 == -1 ){
            result = false;
            break;
        }

        if ( std::isdigit(v6) ){
            this->Number = v6 + 10 * this->Number - '0';
        }else{
            this->scriptFile->ungetChar();
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
    int count = 0;
    token->retrieve(this->String, count);
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
        c = this->scriptFile->getChar();
    }while(std::isspace(c));

    this->scriptFile->ungetChar();
}

void ReadScriptFile::skipLine()
{
    int c;
    do{
        c = this->scriptFile->getChar();
    }while(c != '\n');

    this->scriptFile->pushLineCount();
}

void ReadScriptFile::nextToken()
{

  if ( this->RecursionDepth == -1 )
  {
    this->printError("TReadScriptFile::nextToken: Kein Skript zum Lesen ge");
    this->Token = ENDOFFILE;
    return;
  }

  int v6 = 0;

  this->Number = 0;

  this->Bytes.clear();
  this->String.clear();

  while (true)
  {

      v6 = this->scriptFile->getChar();

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
          this->scriptFile->ungetChar();
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
          this->scriptFile->ungetChar();
          this->retrieveIdentifier();
          return;

      }

      if ( std::isdigit(v6) )
      {
          this->scriptFile->ungetChar();
          this->retrieveNumberOrBytes();
          return;
      }

      if(v6 == '"')
      {
          this->scriptFile->ungetChar();
          this->retrieveString();
          return;
      }

      if(v6 == '[' || v6 == '<' || v6 == '>' || v6 == '-'){
          this->scriptFile->ungetChar();
          ScriptToken *token = new TokenSpecial(*this->scriptFile);
          if(token->retrieve(this->Special) >= 0){
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

  this->printError("TReadScriptFile::nextToken: Ung");
}

void ReadScriptFile::setToken(TOKEN token)
{
    this->Token = token;
}

void ReadScriptFile::error(const std::string &err)
{
  snprintf(this->ErrorString, 0x64u, "error in script-file \"%s\", line %d: %s", this->scriptFile->getFileName().c_str(), this->scriptFile->getLineCount(), err.c_str());

  this->closeAll();
  this->printError(this->ErrorString);

  throw std::logic_error(this->ErrorString);
}

void ReadScriptFile::printError(const std::string &err)
{
    std::cout << err << std::endl;
}

void ReadScriptFile::internalClose(int fileIndex)
{
    int index = fileIndex ? fileIndex : this->RecursionDepth;
    this->Files[index]->resetLineCount();

    if(!this->Files[index]->close()){
        this->printError("TReadScriptFile::close: Fehler %d beim Schlie");
    }

    --this->RecursionDepth;
    this->scriptFile = this->Files[this->RecursionDepth];
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
