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
    }

    return result;
}

