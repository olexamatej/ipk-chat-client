#ifndef INPUTPARSER_H
#define INPUTPARSER_H
#include "packet.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <variant>
#include "connection.h"


#define PACKET_TYPE MsgPacket, JoinPacket, AuthPacket, NullPacket

// Class to parse the input
class Input{
    public:
        void getNewInput(std::string line);
        std::variant<PACKET_TYPE> parseInput(Connection &connection);
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

// Function to get the command type
CommandType getCommandType(std::string line);





#endif