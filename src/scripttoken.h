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

    virtual int retrieveNumber(int &number) { return false; };
    virtual int retrieveString(std::string &str) { return false; };
};

class TokenNumber : public ScriptToken {
public:
    TokenNumber(StreamBuffer &streamBuffer);

    bool retrieveNumber(int &number);

private:
    StreamBuffer& streamBuffer;
};

class TokenString : public ScriptToken {
public:
    TokenString(StreamBuffer &streamBuffer);

    bool retrieveString(std::string &str);

private:
    StreamBuffer& streamBuffer;
};


#endif // SCRIPTTOKEN_H
