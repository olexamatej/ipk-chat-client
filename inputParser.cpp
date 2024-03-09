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


std::variant<PACKET_TYPE> Input::parseInput(){
    CommandType type = getCommandType(this->line);
    std::vector<std::string> arguments;
    std::istringstream iss(this->line);

    static std::string display_name = "";
    static std::string id = "";

    for(std::string s; std::getline(iss, s, ' '); ) {
        arguments.push_back(s);
    }

    // std::cout << "input is " << this->line << std::endl;

    switch(type){
        case CommandType::JOIN:{
            std::cout << "Joining" << std::endl;
            JoinPacket joinPacket(arguments, display_name);
            return joinPacket;
        break;
        }
        case CommandType::AUTH:{
            std::cout << "Authenticating" << std::endl;
            AuthPacket authPacket(arguments);
            id = authPacket.getData()[0];
            display_name = authPacket.getData()[1];
            return authPacket;

        break;
        }
        case CommandType::RENAME:
            if(arguments.size() == 2){
                std::cout << "Renaming" << std::endl;
                display_name = arguments[1];
            }
            else{
                std::cout << "Invalid number of arguments" << std::endl;
                exit(1);
            }
        break;
        case CommandType::HELP:
        break;
        default:{
            std::cout << "Sending message" << std::endl;
            MsgPacket msgPacket(display_name, this->line);
            return msgPacket;
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
