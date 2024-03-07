#include "inputParser.h"


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

void Input::ParseInput(){
    CommandType type = getCommandType(this->line);
    std::vector<std::string> arguments;
    std::istringstream iss(this->line);

    for(std::string s; std::getline(iss, s, ' '); ) {
        arguments.push_back(s);
    }

    std::cout << "input is " << this->line << std::endl;

    // switch(type){
    //     case CommandType::JOIN:
    //     break;
    // }
}

std::string Input::getLine(){
    if(!this->line.ends_with("\n")){
        this->line += "\n";
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
