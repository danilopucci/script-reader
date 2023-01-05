#include "scripttoken.h"

ScriptToken::ScriptToken(StreamBuffer& streamBuffer) :
    streamBuffer(streamBuffer)
{
}

TokenNumber::TokenNumber(StreamBuffer& streamBuffer) :
    ScriptToken(streamBuffer)
{
    this->type = ScriptTokenType::aNUMBER;
}

int TokenNumber::retrieve(int& number){
    number = 0;
    int c = 0;
    int result = 0;

    while(true){

        c = this->streamBuffer.getChar();

        if ( c == -1 ){
            result = -1;
            this->error("retrieveNumber; unexpected end of file");
            break;
        }

        if ( std::isdigit(c) ){
            number = c + 10 * number - '0';
        }else{
            this->streamBuffer.ungetChar();
            break;
        }
    }

    return result;
}

TokenString::TokenString(StreamBuffer& streamBuffer) :
    ScriptToken(streamBuffer)
{
    this->type = ScriptTokenType::aSTRING;
}

int TokenString::retrieve(std::string& str, int &count)
{
    int c = 0;
    int result = 0;
    count = 0;

    c = this->streamBuffer.getChar();

    if(c != '"'){
        result = -1;
        this->error("retrieveString; syntax error; '\"' expected");
        return result;
    }

    while((c = this->streamBuffer.getChar()) != '"'){

        if ( c == '\\' ){
            c = this->streamBuffer.getChar();

            if ( c == 'n' ){
                str.push_back('\n');
            }else{
                str.push_back(c);
            }

        }else{
            //count the number of lines found
            if ( c == '\n' ){
                count++;
            }

            str.push_back(c);
        }

        if(str.length() >= MAX_STRING_LENGHT){
            result = -1;
            this->error("retrieveString; string too long");
            return result;
        }
    }

    return result;
}

TokenCoordinate::TokenCoordinate(StreamBuffer& streamBuffer) :
    TokenNumber(streamBuffer)
{
    this->type = ScriptTokenType::aCOORDINATE;
}

int TokenCoordinate::retrieve(int &x, int &y, int &z)
{
    int result = 0;

    if(this->streamBuffer.getChar() != '['){
        result = -1;
        this->error("retrieveCoordinate; syntax error; '[' expected");
        return result;
    }

    int number = 0;
    int sign = 1;

    this->retrieveSign(sign);
    this->retrieve(number);
    x = number * sign;

    if(this->streamBuffer.getChar() != ','){
        result = -1;
        this->error("retrieveCoordinate; syntax error; ',' expected");
        return result;
    }

    this->retrieveSign(sign);
    this->retrieve(number);
    y = number * sign;

    if(this->streamBuffer.getChar() != ','){
        result = -1;
        this->error("retrieveCoordinate; syntax error; ',' expected");
        return result;
    }

    this->retrieveSign(sign);
    this->retrieve(number);
    z = number * sign;

    if(this->streamBuffer.getChar() != ']'){
        result = -1;
        this->error("retrieveCoordinate; syntax error; ']' expected");
        return result;
    }

    return result;
}

int TokenCoordinate::retrieveSign(int &sign)
{
    int result = 0;
    int c = this->streamBuffer.getChar();

    if ( std::isdigit(c) ){
      sign = 1;
      this->streamBuffer.ungetChar();
    }else{

      if ( c != '-' ){
        result = -1;
        this->error("retrieveSign; syntax error; '-' expected");
        return result;
      }

      sign = -1;
    }

    return result;
}

TokenIdentifier::TokenIdentifier(StreamBuffer& streamBuffer) :
    ScriptToken(streamBuffer)
{
    this->type = ScriptTokenType::aIDENTIFIER;
}

int TokenIdentifier::retrieve(std::string& identifier)
{
    int c = 0;
    int result = 0;

    while(true){
        c = this->streamBuffer.getChar();

        if (identifier.length() >= MAX_IDENTIFIER_LENGHT )
            this->error("identifier too long");

        if ( c == -1 ){
            result = -1;
            break;
        }

        if ( std::isalpha(c) || std::isdigit(c) || c == '_' ){
            identifier.push_back(c);
        }else{
            this->streamBuffer.ungetChar();
            result = -1;
            break;
        }
    }

    return result;
}

TokenSpecial::TokenSpecial(StreamBuffer& streamBuffer) :
    ScriptToken(streamBuffer)
{
    this->type = ScriptTokenType::aSPECIAL;
}

int TokenSpecial::retrieve(char &special)
{
    int c = this->streamBuffer.getChar();
    int result = -1;

    if(c == '['){
        special = c;

        c = this->streamBuffer.getChar();

        if(c == -1){
            result = 0;
            return result;
        }

        if(!std::isdigit(c) && c != '-'){
            this->streamBuffer.ungetChar();
            result = 0;
            return result;
        }

        this->streamBuffer.ungetChar();
        this->streamBuffer.ungetChar();
        result = -1;
        return result;
    }

    if (c == '<' || c == '>')
    {
        special = c;
        this->streamBuffer.ungetChar();
        result = this->retrieveRelationalOperator(special);

        return result;
    }

    if(c == '-'){
        special = c;
        this->streamBuffer.ungetChar();
        result = this->retrieveSeparator(special);

        return result;
    }

    special = c;
    result = 0;
    return result;
}

int TokenSpecial::retrieveRelationalOperator(char &relationalOperator)
{
    int c = this->streamBuffer.getChar();
    int result = 0;

    if ( c == '<' )
    {
        c = this->streamBuffer.getChar();

        if(c == -1){
            result = 0;
            return result;
        }

        if(c == '='){
            relationalOperator = 'L';
            result = 0;
            return result;
        }

        if(c == '>'){
            relationalOperator = 'N';
            result = 0;
            return result;
        }

        relationalOperator = '<';
        this->streamBuffer.ungetChar();
        result = 0;
        return result;
    }

    if ( c == '>' )
    {
        c = this->streamBuffer.getChar();

        if(c == -1){
            result = 0;
            return result;
        }

        if(c == '='){
            relationalOperator = 'G';
            result = 0;
            return result;
        }

        relationalOperator = '>';
        this->streamBuffer.ungetChar();
        result = 0;
        return result;
    }

    result = 0;
    return result;
}

int TokenSpecial::retrieveSeparator(char &separator)
{
    int c = this->streamBuffer.getChar();
    int result = 0;

    if(c == '-'){

        c = this->streamBuffer.getChar();
        if(c == -1){
            result = -1;
            return result;
        }

        if ( c == '>' ){
            separator = 'I';
            result = 0;
            return result;
        }
    }

    result = -1;
    this->streamBuffer.ungetChar();
    return result;
}
