#ifndef STREAMBUFFER_H
#define STREAMBUFFER_H

#include <cstdint>
#include <iostream>

class StreamBuffer
{
public:
    StreamBuffer(){

    }


    virtual int getChar() {
        std::cout << "StreamBuffer::getChar" << std::endl;
        return 0;
    }

    virtual void ungetChar(){
        std::cout << "StreamBuffer::ungetChar" << std::endl;
    }

    virtual char nextChar() {
        char ch = getChar();
        ungetChar();
        std::cout << "StreamBuffer::nextChar" << std::endl;
        return ch;
    }
};

#endif // STREAMBUFFER_H
