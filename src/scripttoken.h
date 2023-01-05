#ifndef SCRIPTTOKEN_H
#define SCRIPTTOKEN_H

#include "streambuffer.h"

enum ScriptTokenType{
    aENDOFFILE        = 0,
    aIDENTIFIER       = 1,
    aNUMBER           = 2,
    aSTRING           = 3,
    aBYTES            = 4,
    aCOORDINATE       = 5,
    aSPECIAL          = 6
};


class ScriptToken{

public:
    ScriptToken(StreamBuffer& streamBuffer);

    virtual int retrieve(int &number) { return 0; };
    virtual int retrieve(std::string &str, int& count) { return 0; };
    virtual int retrieve(int &x, int &y, int &z){ return 0; };
    virtual int retrieve(std::string& identifier){ return 0; }
    virtual int retrieve(char &special){ return 0; }

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

#endif // SCRIPTTOKEN_H
