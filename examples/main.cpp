#include <iostream>

#include "readscriptfile.h"

int main(int argc, char* argv[])
{
    //read current dir
    std::string currentDir(argv[0]);
    std::string npcFilename = currentDir + "/alice.npc";

    std::cout << "NPC with " << npcFilename << " is going to be loaded" << std::endl;

    // NPC parameters to be read
    std::string Name;
    int Sex;
    int Type;
    int outfitId;
    std::vector<uint8_t> Outfit;
    int posx;
    int posy;
    int posz;
    int WalkRadius;
    int Speed;

    //aux variables to read NPC
    std::string identifier;
    bool success = true;
    ReadScriptFile* script = new ReadScriptFile();

    try {
        //open file
        script->open(npcFilename);

        while (1)
        {
            script->nextToken();

            //if it reaches end of file, skip
            if ( script->Token == TOKEN_ENDOFFILE )
                break;

            // should read identifier
            if ( script->Token != TOKEN_IDENTIFIER )
                script->error("identifier expected");

            identifier = script->getIdentifier();

            //read the symbol and advance
            script->readSymbol('=');

            if ( !identifier.compare("name") )
            {
                Name = script->readString();
            }
            else if ( !identifier.compare("sex") )
            {
                std::string sex = script->readIdentifier();
                Sex = 255;

                if ( !sex.compare("male") )
                    Sex = 1;

                if ( !sex.compare("female") )
                    Sex = 2;

                if ( Sex == 255 )
                    script->error("unknown constant");
            }
            else if ( !identifier.compare("type") )
            {
                Type = script->readNumber();
            }
            else if ( !identifier.compare("outfit") ){
                script->readSymbol('(');
                outfitId = script->readNumber();
                script->readSymbol(',');

                if ( outfitId ){
                    Outfit = script->readBytesequence();
                }

                script->readSymbol(')');
            }
            else if ( !identifier.compare("home") )
            {
                script->readCoordinate(posx, posy, posz);
            }
            else if ( !identifier.compare("radius") ){
                WalkRadius = script->readNumber();
            }
            else if ( !identifier.compare("speed") )
            {
                Speed = script->readNumber();
            }
        }

        script->close();

    }  catch (std::exception& e) {
        std::cout << "exception ocurred: " << e.what() << std::endl;
        success = false;
    }

    if(success){
        std::cout << "Name is " << Name << std::endl;
        std::cout << "Sex is " << Sex << " 1 = male, 2 = female" << std::endl;
        std::cout << "Type is " << Type << std::endl;
        std::cout << "OutfitID is " << outfitId << std::endl;
        std::cout << "Coordinate is " << posx << " ;" << posy << " ;" << posz << std::endl;
        std::cout << "WalkRadius is " << WalkRadius << std::endl;
        std::cout << "Speed is " << Speed << std::endl;
    }
    return 0;
}


/*

Name = "Alice"
Sex = female
Type = 2
Outfit = (32,0-0-0-0)
Coordinate = [1032,1033,6]
WalkRadius = 14
Speed = 10
*/
