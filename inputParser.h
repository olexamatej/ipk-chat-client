#ifndef INPUTPARSER_H
#define INPUTPARSER_H
#include "packet_tcp.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <variant>
#include "connection.h"


#define PACKET_TYPE MsgPacket, JoinPacket, AuthPacket, NullPacket

class Input{
    public:
        void getNewInput(std::string line);
        std::variant<PACKET_TYPE> parseInput();
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