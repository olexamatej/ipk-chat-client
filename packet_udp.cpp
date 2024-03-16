

#include "packet_udp.h"

AuthPacket::AuthPacket(const std::string& id, const std::string& dname, const std::string& secret){
    this->id = id;
    this->dname = dname;
    this->secret = secret;    
}

std::string AuthPacket::serialize(const Connection connection){
    std::vector<uint8_t> packet;

     uint16_t id_num = std::stoi(id);

    // Start with 0x02
    packet.push_back(0x02);

    // Add MessageID
    packet.push_back(id_num >> 8);  // High byte
    packet.push_back(id_num & 0xFF);  // Low byte

    // Add Username
    for (char c : id) {
        packet.push_back(c);
    }

    // Add 0 separator
    packet.push_back(0);

    // Add DisplayName
    for (char c : dname) {
        packet.push_back(c);
    }

    // Add 0 separator
    packet.push_back(0);

    // Add Secret
    for (char c : secret) {
        packet.push_back(c);
    }

    // Add 0 separator
    packet.push_back(0);

    // Convert packet to string
    std::string packet_str(packet.begin(), packet.end());

    return packet_str;
}


JoinPacket::JoinPacket(const std::vector<std::string>& arguments, const std::string& dname){
    if(arguments.size() != 2){
        //TODO throw exception
        std::cout << "Invalid number of arguments" << std::endl;
        exit(1);
    }
    this->id = arguments[1];
    this->display_name = dname;
}

std::string JoinPacket::serialize(){
    std::vector<uint8_t> packet;

    uint16_t id_num = std::stoi(id);

    // Start with 0x03
    packet.push_back(0x03);

    // Add MessageID
    packet.push_back(id_num >> 8);  // High byte
    packet.push_back(id_num & 0xFF);  // Low byte

    // Add Username
    for (char c : id) {
        packet.push_back(c);
    }

    // Add 0 separator
    packet.push_back(0);

    // Add DisplayName
    for (char c : display_name) {
        packet.push_back(c);
    }

    // Add 0 separator
    packet.push_back(0);

    // Convert packet to string
    std::string packet_str(packet.begin(), packet.end());

    return packet_str;
}

MessagePacket::MessagePacket(const std::vector<std::string>& arguments, const std::string& display_name){
    if(arguments.size() < 3){
        //TODO throw exception
        std::cout << "Invalid number of arguments" << std::endl;
        exit(1);
    }
    this->id = arguments[1];
    this->message = arguments[2];
    this->display_name = display_name;
}

std::string MessagePacket::serialize(){
    std::vector<uint8_t> packet;

    uint16_t id_num = std::stoi(id);

    // Start with 0x04
    packet.push_back(0x04);

    // Add MessageID
    packet.push_back(id_num >> 8);  // High byte
    packet.push_back(id_num & 0xFF);  // Low byte

    // Add Username
    for (char c : id) {
        packet.push_back(c);
    }

    // Add 0 separator
    packet.push_back(0);

    // Add DisplayName
    for (char c : display_name) {
        packet.push_back(c);
    }

    // Add 0 separator
    packet.push_back(0);

    // Add Message
    for (char c : message) {
        packet.push_back(c);
    }

    // Add 0 separator
    packet.push_back(0);

    // Convert packet to string
    std::string packet_str(packet.begin(), packet.end());

    return packet_str;
}

ErrorPacket::ErrorPacket(const std::vector<std::string>& arguments){
    if(arguments.size() < 3){
        //TODO throw exception
        std::cout << "Invalid number of arguments" << std::endl;
        exit(1);
    }
    this->id = arguments[1];
    this->display_name = arguments[2];
    this->message = arguments[3];
}


std::string ErrorPacket::serialize(){
    std::vector<uint8_t> packet;

    uint16_t id_num = std::stoi(id);

    // Start with 0x05
    packet.push_back(0xFE);

    // Add MessageID
    packet.push_back(id_num >> 8);  // High byte
    packet.push_back(id_num & 0xFF);  // Low byte

    // Add Username
    for (char c : id) {
        packet.push_back(c);
    }

    // Add 0 separator
    packet.push_back(0);

    // Add DisplayName
    for (char c : display_name) {
        packet.push_back(c);
    }

    // Add 0 separator
    packet.push_back(0);

    // Add Message
    for (char c : message) {
        packet.push_back(c);
    }

    // Add 0 separator
    packet.push_back(0);

    // Convert packet to string
    std::string packet_str(packet.begin(), packet.end());

    return packet_str;
}


ReplyPacket::ReplyPacket(const std::vector<std::string>& arguments){
    if(arguments.size() < 3){
        //TODO throw exception
        std::cout << "Invalid number of arguments" << std::endl;
        exit(1);
    }
    this->id = arguments[1];
    this->display_name = arguments[2];
    this->message = arguments[3];
}

std::string ReplyPacket::serialize(){
    std::vector<uint8_t> packet;

    uint16_t id_num = std::stoi(id);

    // Start with 0x06
    packet.push_back(0x06);

    // Add MessageID
    packet.push_back(id_num >> 8);  // High byte
    packet.push_back(id_num & 0xFF);  // Low byte

    // Add Username
    for (char c : id) {
        packet.push_back(c);
    }

    // Add 0 separator
    packet.push_back(0);

    // Add DisplayName
    for (char c : display_name) {
        packet.push_back(c);
    }

    // Add 0 separator
    packet.push_back(0);

    // Add Message
    for (char c : message) {
        packet.push_back(c);
    }

    // Add 0 separator
    packet.push_back(0);

    // Convert packet to string
    std::string packet_str(packet.begin(), packet.end());

    return packet_str;
}

