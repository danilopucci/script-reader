#ifndef SCRIPTTOKEN_H
#define SCRIPTTOKEN_H

#include "streambuffer.h"

#include <stdexcept>
#include <vector>
#include <string>

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

    virtual int retrieve(int &number) { return 0; };
    virtual int retrieve(std::string &str, int& count) { return 0; };
    virtual int retrieve(int &x, int &y, int &z){ return 0; };
    virtual int retrieve(std::string& identifier){ return 0; }
    virtual int retrieve(char &special){ return 0; }
    virtual int retrieve(std::vector<uint8_t> &bytes){ return 0; }
    virtual int retrieve(int &number, std::vector<uint8_t> &bytes){ return 0; }

    void error(const std::string& err){ throw std::logic_error(err); }

    ScriptTokenType type;

protected:
    StreamBuffer& streamBuffer;
};


class TokenNumber : public ScriptToken {

public:
    TokenNumber(StreamBuffer &streamBuffer);

    int retrieve(int &number);
};


class TokenString : public ScriptToken {

public:
    TokenString(StreamBuffer &streamBuffer);

    int retrieve(std::string &str, int &count);

private:
    const int MAX_STRING_LENGHT = 4000;
};


class TokenCoordinate : public TokenNumber {

public:
    TokenCoordinate(StreamBuffer &streamBuffer);

    int retrieve(int &x, int &y, int &z);
    int retrieve(int &number) { return TokenNumber::retrieve(number); }

private:
    int retrieveSign(int &sign);
};


class TokenIdentifier : public ScriptToken {

public:
    TokenIdentifier(StreamBuffer &streamBuffer);

    int retrieve(std::string& identifier);

private:
    const int MAX_IDENTIFIER_LENGHT = 30;
};


class TokenSpecial : public ScriptToken {

public:
    TokenSpecial(StreamBuffer &streamBuffer);

    int retrieve(char &special);

private:
    int retrieveRelationalOperator(char &relationalOperator);
    int retrieveSeparator(char &separator);
};


class TokenBytes : public TokenNumber {

public:
    TokenBytes(StreamBuffer &streamBuffer);

    int retrieve(std::vector<uint8_t> &bytes);
    int retrieve(int &number) { return TokenNumber::retrieve(number); }
};


class TokenGenericNumber : public ScriptToken {

public:
    TokenGenericNumber(StreamBuffer &streamBuffer);

    int retrieve(int &number, std::vector<uint8_t> &bytes);

private:
    TokenNumber tokenNumber;
    TokenBytes tokenBytes;
};

#endif // SCRIPTTOKEN_H
