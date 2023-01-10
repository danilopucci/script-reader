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

#ifndef SCRIPTTOKEN_H
#define SCRIPTTOKEN_H

#include "streambuffer.h"

#include <stdexcept>
#include <vector>
#include <string>
#include <memory>

class ScriptToken;
typedef std::shared_ptr<ScriptToken> ScriptTokenPtr;

enum ScriptTokenType{
    TOKEN_ENDOFFILE        = 0,
    TOKEN_IDENTIFIER       = 1,
    TOKEN_NUMBER           = 2,
    TOKEN_STRING           = 3,
    TOKEN_BYTES            = 4,
    TOKEN_COORDINATE       = 5,
    TOKEN_SPECIAL          = 6
};


class ScriptToken{

public:
    ScriptToken(StreamBuffer& streamBuffer);

    virtual bool retrieve(int &number) { return 0; };
    virtual bool retrieve(std::string &str, int& count) { return 0; };
    virtual bool retrieve(int &x, int &y, int &z){ return 0; };
    virtual bool retrieve(std::string& identifier){ return 0; }
    virtual bool retrieve(char &special){ return 0; }
    virtual bool retrieve(std::vector<uint8_t> &bytes){ return 0; }
    virtual bool retrieve(int &number, std::vector<uint8_t> &bytes){ return 0; }

    void error(const std::string& err){ throw std::logic_error(err); }

    ScriptTokenType type;

protected:
    StreamBuffer& streamBuffer;
};


class TokenNumber : public ScriptToken {

public:
    TokenNumber(StreamBuffer &streamBuffer);

    bool retrieve(int &number);
};


class TokenString : public ScriptToken {

public:
    TokenString(StreamBuffer &streamBuffer);

    bool retrieve(std::string &str, int &count);

private:
    const int MAX_STRING_LENGHT = 4000;
};


class TokenCoordinate : public TokenNumber {

public:
    TokenCoordinate(StreamBuffer &streamBuffer);

    bool retrieve(int &x, int &y, int &z);
    bool retrieve(int &number) { return TokenNumber::retrieve(number); }

private:
    bool retrieveSign(int &sign);
};


class TokenIdentifier : public ScriptToken {

public:
    TokenIdentifier(StreamBuffer &streamBuffer);

    bool retrieve(std::string& identifier);

private:
    const int MAX_IDENTIFIER_LENGHT = 30;
};


class TokenSpecial : public ScriptToken {

public:
    TokenSpecial(StreamBuffer &streamBuffer);

    bool retrieve(char &special);

private:
    bool retrieveRelationalOperator(char &relationalOperator);
    bool retrieveSeparator(char &separator);
};


class TokenBytes : public TokenNumber {

public:
    TokenBytes(StreamBuffer &streamBuffer);

    bool retrieve(std::vector<uint8_t> &bytes);
    bool retrieve(int &number) { return TokenNumber::retrieve(number); }
};


class TokenGenericNumber : public ScriptToken {

public:
    TokenGenericNumber(StreamBuffer &streamBuffer);

    bool retrieve(int &number, std::vector<uint8_t> &bytes);

private:
    TokenNumber tokenNumber;
    TokenBytes tokenBytes;
};

#endif // SCRIPTTOKEN_H
