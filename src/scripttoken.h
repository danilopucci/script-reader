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
    ScriptToken();

    ScriptTokenType type;

    virtual int retrieveNumber(int &number) { return 0; };
    virtual int retrieveString(std::string &str) { return 0; };
    virtual int retrieveCoordinate(int &x, int &y, int &z){ return 0; };

    void error(const std::string& err){ throw std::logic_error(err); }
};


class TokenNumber : public ScriptToken {
public:
    TokenNumber(StreamBuffer &streamBuffer);

    int retrieveNumber(int &number);

private:
    StreamBuffer& streamBuffer;
};


class TokenString : public ScriptToken {
public:
    TokenString(StreamBuffer &streamBuffer);

    int retrieveString(std::string &str);

private:
    StreamBuffer& streamBuffer;
    const int MAX_STRING_LENGHT = 4000;
};


class TokenCoordinate : public TokenNumber {
public:
    TokenCoordinate(StreamBuffer &streamBuffer);

    int retrieveCoordinate(int &x, int &y, int &z);

private:
    int retrieveSign(int &sign);
private:
    StreamBuffer& streamBuffer;
};

#endif // SCRIPTTOKEN_H
