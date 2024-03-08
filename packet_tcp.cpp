#include "packet_tcp.h"

const std::string SP = " ";
const std::string IS = " IS ";
const std::string AS = " AS ";
const std::string CRLF = "\r\n";

MsgPacket::MsgPacket(const std::string& dname, const std::string& content) {
    this->dname = dname;
    this->content = content;
}

std::string MsgPacket::serialize() {
    return "MSG FROM" + SP + this->dname + IS + this->content + CRLF; 
}


JoinPacket::JoinPacket(const std::string& dname, const std::string& id) {
    this->dname = dname;
    this->id = id;
}

std::string JoinPacket::serialize() {
    return "JOIN" + SP + this->id + AS + this->dname + CRLF;
}

AuthPacket::AuthPacket(const std::string& id, const std::string& dname, const std::string& secret) {
    this->id = id;
    this->dname = dname;
    this->secret = secret;
}

std::string AuthPacket::serialize() {
    return "AUTH" + SP + id + AS + dname + " using " + secret + CRLF;
}

ErrorPacket::ErrorPacket(const std::string& dname, const std::string& content) {
    this->dname = dname;
    this->content = content;
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