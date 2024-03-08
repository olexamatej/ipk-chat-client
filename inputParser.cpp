#include "inputParser.h"
#include "packet_tcp.h"

CommandType getCommandType(std::string line){
    if(line.find("/join",0) == 0){
        return CommandType::JOIN;
    }
    if(line.find("/auth",0) == 0){
        return CommandType::AUTH;
    }
    if(line.find("/rename",0) == 0){
        return CommandType::RENAME;
    }
    if(line.find("/help",0) == 0){
        return CommandType::HELP;
    }
    return CommandType::ELSE;
}

std::string Input::parseInput(){
    CommandType type = getCommandType(this->line);
    std::vector<std::string> arguments;
    std::istringstream iss(this->line);

    for(std::string s; std::getline(iss, s, ' '); ) {
        arguments.push_back(s);
    }

    // std::cout << "input is " << this->line << std::endl;

    switch(type){
        case CommandType::JOIN:{
            std::cout << "Joining" << std::endl;
            JoinPacket joinPacket(arguments[1], arguments[2]);
            return joinPacket.serialize();
        break;
        }
        case CommandType::AUTH:{
            std::cout << "Authenticating" << std::endl;
            AuthPacket authPacket(arguments[1], arguments[2], arguments[3]);
            return authPacket.serialize();
        break;
        }
        case CommandType::RENAME:
        break;
        case CommandType::HELP:
        break;
        default:{
            std::cout << "Sending message" << std::endl;
            MsgPacket msgPacket("Matej", this->line);
            return msgPacket.serialize();
        }
        break;
    }
}

//TODO: implement this
std::string Input::getLine(){
    if(!this->line.ends_with("\r\n")){
        this->line += "\r\n";
    }
    return this->line;
}


void Input::getNewInput(std::string line){
    this->line = line;
}

// int main() {
//     std::string line;
//     Input userInput;
//     while(std::getline(std::cin, line)) {
//         userInput.getNewInput(line);
//         userInput.ParseInput();
//     }
//     return 0;
// }
