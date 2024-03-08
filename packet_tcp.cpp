#include "packet_tcp.h"

const std::string SP = " ";
const std::string IS = " IS ";
const std::string AS = " AS ";
const std::string CRLF = "\r\n";

MsgPacket::MsgPacket(const std::string& dname, const std::string& content) {
    if(dname == "" || content == ""){
        //TODO throw exception
        std::cout << "Dname or content is empty" << std::endl;
        exit(1);
    }
    this->dname = dname;
    this->content = content;
}

std::string MsgPacket::serialize() {
    return "MSG FROM" + SP + this->dname + IS + this->content + CRLF; 
}


JoinPacket::JoinPacket(const std::vector<std::string>& arguments, const std::string& dname) {
    if(arguments.size() != 2){
        //TODO throw exception
        std::cout << "Invalid number of arguments" << std::endl;
        exit(1);
    }
    this->id = arguments[1];
    this->dname = dname;
}

std::string JoinPacket::serialize() {
    if(this->dname == ""){
        //TODO throw exception
        std::cout << "Dname is empty" << std::endl;
        exit(1);
    }
    return "JOIN" + SP + this->id + AS + this->dname + CRLF;
}

AuthPacket::AuthPacket(const std::vector<std::string>& arguments){
    if(arguments.size() != 4){
        //TODO throw exception
        std::cout << "Invalid number of arguments" << std::endl;
        exit(1);
    }
    this->id = arguments[1];
    this->dname = arguments[2];
    this->secret = arguments[3];
}

std::vector<std::string> AuthPacket::getData() {
    std::vector<std::string> data;
    data.push_back(id);
    data.push_back(dname);
    data.push_back(secret);
    return data;
}

std::string AuthPacket::serialize() {
    return "AUTH" + SP + id + AS + dname + " USING " + secret + CRLF;
}

ErrorPacket::ErrorPacket(const std::vector<std::string>& arguments) {
    if(arguments.size() != 3){
        //TODO throw exception
        std::cout << "Invalid number of arguments" << std::endl;
        exit(1);
    }
    this->dname = arguments[1];
    this->content = arguments[2];
}

std::string ErrorPacket::serialize() {
    return "ERR FROM" + SP + dname + IS + content + CRLF;
}
 
// int main() {
//     std::string test = "Test string i want to serialize";
//     std::string dname = "Matej";

//     MsgPacket msg(dname, test);
//     std::cout << msg.serialize();

//     return 0;
// }