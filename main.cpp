#include <iostream>
#include <vector>
#include <string>
#include <sstream>

class Input{
    public:
        Input(std::string line);
        // ~Input();
        void ParseInput();
    protected:
        std::string line;
};

Input::Input(std::string line){
    this->line = line;
}

enum class CommandType {
    AUTH,
    JOIN,
    RENAME,
    HELP,
    ELSE
};

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

    switch(type){
        case CommandType::JOIN:
        break;
    }
}

int main() {
    std::string line;

    std::getline(std::cin, line);
    Input userInput(line);
    
    userInput.ParseInput();

    // while(std::getline(std::cin, line)) {
    //         if(Input.find("/join",0) == 0){
    //             std::cout << "vlaky";
    //         }          
    //     }




    return 0;
}
