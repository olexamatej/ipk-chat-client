#ifndef INPUTPARSER_H
#define INPUTPARSER_H
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

class Input{
    public:
        void getNewInput(std::string line);
        std::string parseInput();
        std::string getLine();
    protected:
        std::string line;
};

enum class CommandType {
    AUTH,
    JOIN,
    RENAME,
    HELP,
    ELSE
};

CommandType getCommandType(std::string line);





#endif