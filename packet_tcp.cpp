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

MsgPacket::MsgPacket(const std::vector<uint8_t> data){

}

std::string MsgPacket::serialize(Connection &connection) {
    if(connection.protocol == Connection::Protocol::TCP){
        return "MSG" + SP + this->dname + IS + this->content + CRLF;
    }
    else if(connection.protocol == Connection::Protocol::UDP){
        std::vector<uint8_t> packet;

        uint16_t id_num = connection.message_id++;

        // Start with 0x04
        packet.push_back(0x04);

        // Add MessageID
        packet.push_back(id_num >> 8);  // High byte
        packet.push_back(id_num & 0xFF);  // Low byte

        // Add DisplayName
        for (char c : this->dname) {
            packet.push_back(c);
        }

        // Add 0 separator
        packet.push_back(0);

        // Add Message
        for (char c : this->content) {
            packet.push_back(c);
        }

        // Add 0 separator
        packet.push_back(0);

        // Convert packet to string
        std::string packet_str(packet.begin(), packet.end());

        return packet_str;
    }

    std::cout<< "Invalid protocol" << std::endl;
    exit(1);
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


std::string JoinPacket::serialize(Connection &connection) {
    if(this->dname == ""){
        //TODO throw exception
        std::cout << "Dname is empty" << std::endl;
        exit(1);
    }

    if(connection.protocol == Connection::Protocol::TCP){
        return "JOIN" + SP + this->id + AS + this->dname + CRLF;
    }

    else if(connection.protocol == Connection::Protocol::UDP){
        std::vector<uint8_t> packet;

        uint16_t id_num = connection.message_id++;

        packet.push_back(0x03);

        // Add MessageID
        packet.push_back(id_num >> 8);  // High byte
        packet.push_back(id_num & 0xFF);  // Low byte

        // Add Username
        for (char c : connection.id) {
            packet.push_back(c);
        }

        // Add 0 separator
        packet.push_back(0);

        // Add DisplayName
        for (char c : this->dname) {
            packet.push_back(c);
        }

        // Add 0 separator
        packet.push_back(0);

        // Convert packet to string
        std::string packet_str(packet.begin(), packet.end());

        return packet_str;
    }
    std::cout<< "Invalid protocol" << std::endl;
    exit(1);
}

AuthPacket::AuthPacket(const std::vector<std::string>& arguments){
    if(arguments.size() != 4){
        //TODO throw exception
        std::cout << "Invalid number of arguments" << std::endl;
        return;
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

std::string AuthPacket::serialize(Connection &connection) {
    if(connection.protocol == Connection::Protocol::TCP){
        return "AUTH" + SP + id + AS + dname + " USING " + secret + CRLF;
    }
    else if(connection.protocol == Connection::Protocol::UDP){
        std::vector<uint8_t> packet;

        uint16_t id_num = connection.message_id++;

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
    std::cout<< "Invalid protocol" << std::endl;
    exit(1);

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
ErrorPacket::ErrorPacket(const std::vector<uint8_t> data){
    uint16_t MessageID = (data[1] << 8) + data[2];
    //displayname until 0 byte
    int i = 3;
    for(; data[i] != 0; i++){
        this->dname += data[i];
    }
    i++;

    for(; data[i] != 0; i++){
        this->content += data[i];
    }


}

ReplyPacket::ReplyPacket(const std::vector <uint8_t> data){
    uint16_t MessageID = (data[1] << 8) + data[2];
    this->success = data[3];
    uint16_t Ref_messageID = (data[4] << 8) + data[5];
    this->content = std::string(data.begin() + 6, data.end());

}

ConfirmPacket::ConfirmPacket(const std::vector<uint8_t> data){
    if(data.size() != 3){
        //TODO throw exception
        std::cout << "Invalid number of arguments" << std::endl;
        exit(1);
    }
    this->refID = std::to_string((data[1] << 8) + data[2]);
}

std::vector<std::string> ConfirmPacket::getData() {
    std::vector<std::string> data;
    data.push_back(refID);
    return data;
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



std::variant<RECV_PACKET_TYPE> ReceiveParser(const std::string data, Connection &connection){
    if(connection.protocol == Connection::Protocol::TCP){
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
    else if(connection.protocol == Connection::Protocol::UDP){
        // Convert data to vector of bytes
        std::vector<uint8_t> data_bytes(data.begin(), data.end());
        uint8_t first_byte = data_bytes[0];
        switch(first_byte){
            case 0x00:{
                std::cout << "PRISIEL CONFIRM PACKET" << std::endl;
                ConfirmPacket confirm_packet(data_bytes);
                return confirm_packet;
                break;
                }
            case 0x01:{
                std::cout << "PRISIEL REPLY PACKET" << std::endl;
                ReplyPacket reply_packet(data_bytes);
                return reply_packet;
                break;
                }
            case 0xFE:{
                ErrorPacket error_packet(data_bytes);
                return error_packet;
                break;
                }
            case 0x04:{
                MsgPacket msg_packet(data_bytes);
                return msg_packet;
            }
            default:
                //TODO Error
                std::cout << "Invalid packet, error in switchcase" << std::endl;
                std::cout << "First byte: " << data << std::endl;
                exit(1);
        }

    }
}



std::string ByePacket::serialize() {
    return "BYE" + CRLF;
}


// int main() {
//     std::string test = "Test string i want to serialize";
//     std::string dname = "Matej";

//     MsgPacket msg(dname, test);
//     std::cout << msg.serialize();

//     return 0;
// }