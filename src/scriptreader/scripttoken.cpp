/****************************************************************************
 *
 *  Copyright (c) 2023, Danilo Pucci
 *
 *  This file is part of the MMORPG Script Reader project.
 *
 *  Source Code:
 *  https://github.com/danilopucci/script-reader/
 *
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files
 *  (the "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so,subject to the
 *  following conditions:
 *
 *  + The above copyright notice and this permission notice shall be included
 *    in all copies or substantial portions of the Software.
 *  + Credit is appreciated, but not required, if you find this project
 *    useful enough to include in your application, product, device, etc.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 *  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ***************************************************************************/
#include "scripttoken.h"

ScriptToken::ScriptToken(StreamBuffer& streamBuffer) :
    streamBuffer(streamBuffer)
{
}

TokenNumber::TokenNumber(StreamBuffer& streamBuffer) :
    ScriptToken(streamBuffer)
{
    this->type = ScriptTokenType::TOKEN_NUMBER;
}

bool TokenNumber::retrieve(int& number){
    number = 0;
    int c = 0;
    bool result = false;

    while(true){

        c = this->streamBuffer.getChar();

        if ( c == -1 ){
            result = false;
            this->error("unexpected end of file");
            break;
        }

        if ( std::isdigit(c) ){
            number = c + 10 * number - '0';
            result = true;
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
    this->type = ScriptTokenType::TOKEN_STRING;
}

bool TokenString::retrieve(std::string& str, int &count)
{
    int c = 0;
    bool result = false;
    count = 0;

    c = this->streamBuffer.getChar();

    if(c != '"'){
        result = false;
        this->error("syntax error; '\"' expected");
        return result;
    }

    while((c = this->streamBuffer.getChar()) != '"'){

        if(c == -1){
            result = false;
            this->error("unexpected end of file");
            return result;
        }

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
            result = false;
            this->error("string too long");
            return result;
        }
    }

    if(str.length() > 0){
        result = true;
    }

    return result;
}

TokenCoordinate::TokenCoordinate(StreamBuffer& streamBuffer) :
    TokenNumber(streamBuffer)
{
    this->type = ScriptTokenType::TOKEN_COORDINATE;
}

bool TokenCoordinate::retrieve(int &x, int &y, int &z)
{
    bool result = true;

    if(this->streamBuffer.getChar() != '['){
        result = false;
        this->error("syntax error; '[' expected");
        return result;
    }

    int number = 0;
    int sign = 1;

    if(!this->retrieveSign(sign)){
        result = false;
        this->error("unexpected end of file");
        return result;
    }

    if(!this->retrieve(number)){
        result = false;
        this->error("unexpected end of file");
        return result;
    }
    x = number * sign;


    if(this->streamBuffer.getChar() != ','){
        result = false;
        this->error("syntax error; ',' expected");
        return result;
    }

    if(!this->retrieveSign(sign)){
        result = false;
        this->error("unexpected end of file");
        return false;
    }

    if(!this->retrieve(number)){
        result = false;
        this->error("unexpected end of file");
        return result;
    }
    y = number * sign;


    if(this->streamBuffer.getChar() != ','){
        result = false;
        this->error("syntax error; ',' expected");
        return result;
    }


    if(!this->retrieveSign(sign)){
        result = false;
        this->error("unexpected end of file");
        return result;
    }

    if(!this->retrieve(number)){
        result = false;
        this->error("unexpected end of file");
        return result;
    }
    z = number * sign;


    if(this->streamBuffer.getChar() != ']'){
        result = false;
        this->error("syntax error; ']' expected");
        return result;
    }

    return result;
}

bool TokenCoordinate::retrieveSign(int &sign)
{
    bool result = false;
    int c = this->streamBuffer.getChar();

    if(c == -1){
        result = false;
        this->error("unexpected end of file");
    }

    if ( std::isdigit(c) ){
      result = true;
      sign = 1;
      this->streamBuffer.ungetChar();
    }else{

      if ( c != '-' ){
        result = false;
        this->error("syntax error; '-' expected");
        return result;
      }

      result = true;
      sign = -1;
    }

    return result;
}

TokenIdentifier::TokenIdentifier(StreamBuffer& streamBuffer) :
    ScriptToken(streamBuffer)
{
    this->type = ScriptTokenType::TOKEN_IDENTIFIER;
}

bool TokenIdentifier::retrieve(std::string& identifier)
{
    int c = 0;
    bool result = false;

    while(true){
        c = this->streamBuffer.getChar();

        if (identifier.length() >= MAX_IDENTIFIER_LENGHT )
            this->error("identifier too long");

        if ( c == -1 ){
            result = false;
            break;
        }

        if ( std::isalpha(c) || std::isdigit(c) || c == '_' ){
            identifier.push_back(c);
        }else{
            this->streamBuffer.ungetChar();
            result = false;
            break;
        }
    }

    if(identifier.length() > 0){
        result = true;
    }

    return result;
}

TokenSpecial::TokenSpecial(StreamBuffer& streamBuffer) :
    ScriptToken(streamBuffer)
{
    this->type = ScriptTokenType::TOKEN_SPECIAL;
}

bool TokenSpecial::retrieve(char &special)
{
    int c = this->streamBuffer.getChar();
    bool result = false;

    if(c == '['){
        special = c;

        c = this->streamBuffer.getChar();

        if(c == -1){
            result = true;
            return result;
        }

        if(!std::isdigit(c) && c != '-'){
            this->streamBuffer.ungetChar();
            result = true;
            return result;
        }

        this->streamBuffer.ungetChar();
        this->streamBuffer.ungetChar();
        result = false;
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
    result = true;
    return result;
}

bool TokenSpecial::retrieveRelationalOperator(char &relationalOperator)
{
    int c = this->streamBuffer.getChar();
    bool result = false;

    if ( c == '<' )
    {
        c = this->streamBuffer.getChar();

        if(c == -1){
            result = true;
            return result;
        }

        if(c == '='){
            relationalOperator = 'L';
            result = true;
            return result;
        }

        if(c == '>'){
            relationalOperator = 'N';
            result = true;
            return result;
        }

        relationalOperator = '<';
        this->streamBuffer.ungetChar();
        result = true;
        return result;
    }

    if ( c == '>' )
    {
        c = this->streamBuffer.getChar();

        if(c == -1){
            result = true;
            return result;
        }

        if(c == '='){
            relationalOperator = 'G';
            result = true;
            return result;
        }

        relationalOperator = '>';
        this->streamBuffer.ungetChar();
        result = true;
        return result;
    }

    if(c == -1){
        result = true;
        return result;
    }

    result = true;
    return result;
}

bool TokenSpecial::retrieveSeparator(char &separator)
{
    int c = this->streamBuffer.getChar();
    bool result = false;

    if(c == '-'){

        c = this->streamBuffer.getChar();
        if(c == -1){
            result = false;
            return result;
        }

        if ( c == '>' ){
            separator = 'I';
            result = true;
            return result;
        }
    }

    result = false;
    this->streamBuffer.ungetChar();
    return result;
}

TokenBytes::TokenBytes(StreamBuffer &streamBuffer) :
    TokenNumber(streamBuffer)
{
    this->type = ScriptTokenType::TOKEN_BYTES;
}

bool TokenBytes::retrieve(std::vector<uint8_t> &bytes)
{
    bool result = false;
    int number = 0;

    bytes.clear();

    this->retrieve(number);
    bytes.emplace_back(number);

    while(this->streamBuffer.getChar() == '-'){
        this->retrieve(number);
        bytes.emplace_back(number);
        result = true;
    }

    if(!result){
        bytes.clear();
    }

    this->streamBuffer.ungetChar();
    return result;
}

TokenGenericNumber::TokenGenericNumber(StreamBuffer &streamBuffer) :
    tokenBytes(streamBuffer), tokenNumber(streamBuffer), ScriptToken(streamBuffer)
{
    this->type = ScriptTokenType::TOKEN_ENDOFFILE;
}

bool TokenGenericNumber::retrieve(int &number, std::vector<uint8_t> &bytes)
{
    bool result = false;
    int number_ = 0;
    int c = 0;
    std::vector<uint8_t> bytes_;

    if(this->tokenNumber.retrieve(number_)){
        number = number_;
        this->type = ScriptTokenType::TOKEN_NUMBER;
        result = true;
    }

    if(result){
        c = this->streamBuffer.getChar();

        if(c == -1){
            return result;
        }

        if(c == '-'){
            this->type = ScriptTokenType::TOKEN_BYTES;
            bytes_.emplace_back(number_);

            if(this->tokenBytes.retrieve(bytes)){
                bytes_.insert(bytes_.end(), bytes.begin(), bytes.end());
                bytes = bytes_;
                result = true;
                return result;
            }
        }
    }

    this->streamBuffer.ungetChar();
    return result;
}
