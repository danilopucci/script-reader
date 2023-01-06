#ifndef STREAMBUFFER_H
#define STREAMBUFFER_H

#include <cstdint>

class StreamBuffer
{
public:
    StreamBuffer(){

    }

    virtual int getChar() {
        return 0;
    }

    virtual void ungetChar(){

    }

    virtual char nextChar() {
        char ch = getChar();

        if(ch != -1){
            ungetChar();
        }

        return ch;
    }
};

#endif // STREAMBUFFER_H
