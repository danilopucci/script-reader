## MMORPG Script Reader

This repository contains a library to read MMORPG game Script Files.

The file extensions which it can read:

| Extension | Description                 |
|-----------|-----------------------------|
| .npc      | Non-player Character files  |
| .ndb      | NPC behavior database files |
| .db       | Monster Homes files         |
| .evt      | Monster event files         |
| .mon      | Monster data files          |
| .pat      | Patch files                 |
| .dat      | General purpose data files  |
| .usr      | User files                  |
| .srv      | Object types files          |

### Adding to project

* simple copy-and-paste the scriptreader folder to your project 

OR
* add to your CMake project as a library:
    ```
    add_subdirectory(src/scriptreader)
    target_link_libraries(script-reader PRIVATE scriptreader)
    ```

### Using the library

It is only necessary to include readscriptfile.h

* open the file by using ```ReadScriptFile::open("myfile.npc")```
* make a call to ```ReadScriptFile::nextToken()```
* get the read toke type (Identifier, Number, String, Bytes, Coordinate or Special)
* read the token data by calling ```ReadScriptFile::readIdentifier()``` or ```ReadScriptFile::readNumber()``` etc
* get the read token data by calling ```ReadScriptFile::getIdentifier()``` or ```ReadScriptFile::getNumber()``` etc
* store the item data to your code and cycle to ```ReadScriptFile::nextToken()``` until it reaches End-of-file
* close the file by using ```ReadScriptFile::close()```

##### Note
It throws exception in case of error while reading script file, so it may be useful to operate inside a try-catch block

