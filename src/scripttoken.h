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

    virtual int retrieveNumber(int &number) { return 0; };
    virtual int retrieveString(std::string &str) { return 0; };
    virtual int retrieveCoordinate(int &x, int &y, int &z){ return 0; };
    virtual int retrieveIdentifier(std::string& identifier){ return 0; }
    virtual int retrieveSpecial(char &special){ return 0; }

    void error(const std::string& err){ throw std::logic_error(err); }

    ScriptTokenType type;

protected:
    StreamBuffer& streamBuffer;
};


class TokenNumber : public ScriptToken {

public:
    TokenNumber(StreamBuffer &streamBuffer);

    int retrieveNumber(int &number);
};


class TokenString : public ScriptToken {

public:
    TokenString(StreamBuffer &streamBuffer);

    int retrieveString(std::string &str);

private:
    const int MAX_STRING_LENGHT = 4000;
};


class TokenCoordinate : public TokenNumber {

public:
    TokenCoordinate(StreamBuffer &streamBuffer);

    int retrieveCoordinate(int &x, int &y, int &z);

private:
    int retrieveSign(int &sign);
};


class TokenIdentifier : public ScriptToken {

public:
    TokenIdentifier(StreamBuffer &streamBuffer);

    int retrieveIdentifier(std::string& identifier);

private:
    const int MAX_IDENTIFIER_LENGHT = 30;
};


class TokenSpecial : public ScriptToken {

public:
    TokenSpecial(StreamBuffer &streamBuffer);

    int retrieveSpecial(char &special);

private:
    int retrieveRelationalOperator(char &relationalOperator);
    int retrieveSeparator(char &separator);
};

#endif // SCRIPTTOKEN_H
