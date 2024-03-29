#include "inputParser.h"
#include "packet.h"


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


std::variant<PACKET_TYPE> Input::parseInput(Connection &connection){
    CommandType type = getCommandType(this->line);
    std::vector<std::string> arguments;
    std::istringstream iss(this->line);

    std::string display_name = connection.display_name;
    std::string id = connection.id;
    for(std::string s; std::getline(iss, s, ' '); ) {
        arguments.push_back(s);
    }

    // std::cout << "input is " << this->line << std::endl;
    if((display_name == "" || id == "") && type != CommandType::AUTH){
        // std::cerr << "ERR: Please authenticate first" << std::endl;
        return NullPacket();
    }
    switch(type){
        case CommandType::JOIN:{
            //print all arguments
            if(arguments.size() != 2){
                std::cout << "Invalid number of arguments" << std::endl;
                return NullPacket();
            }
            JoinPacket joinPacket(arguments, display_name);
            if(!joinPacket.LegalCheck()){
                return NullPacket();
            }

            return joinPacket;
        break;
        }
        case CommandType::AUTH:{
            if(arguments.size() != 4){
                return NullPacket();
            }
            AuthPacket authPacket(arguments);
            if(!authPacket.LegalCheck()){
                return NullPacket();
            }
            if(connection.id != "" && connection.display_name != ""){
                return NullPacket();
            }
            connection.id = authPacket.getData()[0];
            connection.display_name = authPacket.getData()[1];
            return authPacket;

        break;
        }
        case CommandType::RENAME:
            if(arguments.size() == 2){
                connection.display_name = arguments[1];
                NullPacket nullPacket;
                nullPacket.rename = true;
                return nullPacket;
            }
            else{
                std::cout << "ERR: Invalid number of arguments" << std::endl;
                return NullPacket();
            }
        break;
        case CommandType::HELP:
        break;
        default:{    
            MsgPacket msgPacket(display_name, this->line);
            if(!msgPacket.LegalCheck()){
                return NullPacket();
            }
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
