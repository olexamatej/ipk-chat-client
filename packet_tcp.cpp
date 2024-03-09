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

ErrorPacket::ErrorPacket(const std::vector<std::string> data) {
    if(data.size() != 3){
        //TODO throw exception
        std::cout << "Invalid number of arguments" << std::endl;
        exit(1);
    }
    this->dname = data[1];
    this->content = data[2];

}

ReplyPacket::ReplyPacket(const std::vector<std::string> data) {
    if(data.size() < 4){
        //TODO throw exception
        std::cout << "Invalid number of arguments" << std::endl;
        exit(1);
    }
    if(data[0] + SP + data[2] != "REPLY IS"){
        std::cout << data[0] + SP + data[2] << std::endl;
        std::cout << "Invalid format of reply" << std::endl;
        exit(1);
    } 

    if(data[1] == "OK"){
        this->success = true;
    } else if(data[1] == "NOK"){
        this->success = false;
    } else {
        std::cout << "Invalid format of reply" << std::endl;
        exit(1);
    }

    std::string content = "";
    for(int i = 4; i < data.size(); i++){
        content += data[i] + " ";
    }
    this->content = content;

}
std::vector<std::string>ReplyPacket::getData() {
    std::vector<std::string> data;
    data.push_back(std::to_string(success));
    data.push_back(content);
    return data;
}
std::vector<std::string> ErrorPacket::getData() {
    std::vector<std::string> data;
    data.push_back(dname);
    data.push_back(content);
    return data;
}


std::variant<RECV_PACKET_TYPE> ReceiveParser(const std::string data){
    std::istringstream iss(data);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    if (!tokens.empty()) {
        std::string packetType = tokens[0];
    
        if(packetType == "REPLY"){
            ReplyPacket reply_packet(tokens);
            return reply_packet;
        }
        else if(packetType == "ERROR"){
            ErrorPacket error_packet(tokens);
            return error_packet;
        }
        else {
            MsgPacket msgPacket(tokens[3], tokens[5]);
            return msgPacket;
        }
    }
    //TODO throw exception
    std::cout << "Invalid packet" << std::endl;
    exit(1);
}


// int main() {
//     std::string test = "Test string i want to serialize";
//     std::string dname = "Matej";

//     MsgPacket msg(dname, test);
//     std::cout << msg.serialize();

//     return 0;
// }