#ifndef READSCRIPTFILE_H
#define READSCRIPTFILE_H

#include <cstdint>
#include <string>
#include <stdio.h>

char * strLower(char *a1);
int isSpace(int a1);
int isAlpha(int a1);
bool isDigit(int c);
char * findLast(char *s, char c);

char ErrorString[100];

enum TOKEN{
    ENDOFFILE        = 0,
    IDENTIFIER       = 1,
    NUMBER           = 2,
    STRING           = 3,
    BYTES            = 4,
    COORDINATE       = 5,
    SPECIAL          = 6
};

class ReadScriptFile
{
public:
    ReadScriptFile();

    ~ReadScriptFile();

    void readSymbol(char);
    char * readString();
    char readSpecial();
    int readNumber();
    char *readIdentifier(void);
    void readCoordinate(int &x, int &y, int &z);
    uint8_t *readBytesequence(void);
    void open(char *);
    void nextToken();
    char *getString();
    uint8_t getSpecial();
    int getNumber();
    char *getIdentifier();
    void getCoordinate(int &x, int &y, int &z);
    uint8_t *getBytesequence();
    void error(const std::string &Text);
    void close();

private:
    TOKEN Token;
    char Special;
    int RecursionDepth;
    uint8_t* Bytes;
    char* String;
    int Number;
    int CoordX;
    int CoordY;
    int CoordZ;
    int Line[3];
    FILE* File[3];
    char Filename[3][4096];
};



#endif // READSCRIPTFILE_H
