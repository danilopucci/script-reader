#include "scripttoken.h"

ScriptToken::ScriptToken()
{
}

TokenNumber::TokenNumber(StreamBuffer& streamBuffer) :
    streamBuffer(streamBuffer)
{
    this->type = ScriptTokenType::aNUMBER;
}

int TokenNumber::retrieveNumber(int& number){
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
    streamBuffer(streamBuffer)
{
    this->type = ScriptTokenType::aSTRING;
}

int TokenString::retrieveString(std::string& str)
{
    int c = 0;
    int result = -1;

    c = this->streamBuffer.getChar();

    if(c != '"'){
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
                result++;
            }

            str.push_back(c);
        }

        if(str.length() >= MAX_STRING_LENGHT){
            this->error("retrieveString; string too long");
        }
    }

    return result;
}

TokenCoordinate::TokenCoordinate(StreamBuffer& streamBuffer) :
    streamBuffer(streamBuffer), TokenNumber(streamBuffer)
{
    this->type = ScriptTokenType::aCOORDINATE;
}

int TokenCoordinate::retrieveCoordinate(int &x, int &y, int &z)
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
    this->retrieveNumber(number);
    x = number * sign;

    if(this->streamBuffer.getChar() != ','){
        result = -1;
        this->error("retrieveCoordinate; syntax error; ',' expected");
        return result;
    }

    this->retrieveSign(sign);
    this->retrieveNumber(number);
    y = number * sign;

    if(this->streamBuffer.getChar() != ','){
        result = -1;
        this->error("retrieveCoordinate; syntax error; ',' expected");
        return result;
    }

    this->retrieveSign(sign);
    this->retrieveNumber(number);
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
      }

      sign = -1;
    }

    return result;
}
