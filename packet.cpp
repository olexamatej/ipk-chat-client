#include "packet.h"

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

bool MsgPacket::LegalCheck(){
    if(content.length() > 1400){
        std::cout << "Content is too long" << std::endl;
        return false;
    }
    return true;
}


MsgPacket::MsgPacket(const std::vector<uint8_t> data){
    this->messageID = (data[1] << 8) + data[2];
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

std::vector<std::string> MsgPacket::getData() {
    std::vector<std::string> data;
    data.push_back(dname);
    data.push_back(content);
    data.push_back(std::to_string(messageID));
    return data;
}

std::string MsgPacket::serialize(Connection &connection) {
    if(connection.protocol == Connection::Protocol::TCP){
        return "MSG FROM" + SP + this->dname + IS + this->content + CRLF;
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
    this->id = arguments[1];
    this->dname = dname;
}

bool JoinPacket::LegalCheck(){
    if(this->dname.length() > 20){
        std::cout << "Display name is too long" << std::endl;
        return false;
    }
    if(this->id.length() > 20){
        std::cout << "ID is too long" << std::endl;
        return false;
    }
    return true;
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

        for (char c : this->id) {
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

bool AuthPacket::LegalCheck(){
    if(this->dname.length() > 20){
        std::cout << "Display name is too long" << std::endl;
        return false;
    }
    for(char c : this->dname){
        if(!isalnum(c) && c != '-'){
            std::cout << "ID has invalid characters" << std::endl;
            return false;
        }
    }
    if(this->id.length() > 20){
        std::cout << "ID is too long" << std::endl;
        return false;
    }
    for(char c : this->id){
        if(!isalnum(c) && c != '-'){
            std::cout << "ID has invalid characters" << std::endl;
            return false;
        }
    }
    if(this->secret.length() > 128){
        std::cout << "Secret is too long" << std::endl;
        return false;
    }
    for(char c : this->secret){
        if(!isalnum(c) && c != '-'){
            std::cout << "ID has invalid characters" << std::endl;
            return false;
        }
    }

    return true;
}

AuthPacket::AuthPacket(const std::vector<std::string>& arguments){
    
    this->id = arguments[1];
    this->dname = arguments[3];
    this->secret = arguments[2];
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
    this->dname = data[2];
    this->content = data[4];
}

ErrorPacket::ErrorPacket(std::string content, std::string dname){
    this->content = content;
    this->dname = dname;
}


ErrorPacket::ErrorPacket(const std::vector<uint8_t> data){
    this->messageID = (data[1] << 8) + data[2];
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

std::string ErrorPacket::serialize(Connection &connection){
    if(connection.protocol == Connection::Protocol::TCP){
        return "ERR FROM" + SP + this->dname + IS + this->content + CRLF;
    }
    else if(connection.protocol == Connection::Protocol::UDP){
        //first byte is 0xFE, next 2 bytes are message ID, then display name until 0 byte, then content until 0 byte
        std::vector<uint8_t> packet;
        packet.push_back(0xFE);
        uint16_t id_num = connection.message_id++;
        packet.push_back(id_num >> 8);  // High byte
        packet.push_back(id_num & 0xFF);  // Low byte

        for (char c : this->dname) {
            packet.push_back(c);
        }

        // Add 0 separator
        packet.push_back(0);

        for (char c : this->content) {
            packet.push_back(c);
        }

        // Add 0 separator
        packet.push_back(0);

        std::string packet_str(packet.begin(), packet.end());
        return packet_str;
    }
    std::cout<< "Invalid protocol" << std::endl;
    exit(1);

}

ReplyPacket::ReplyPacket(const std::vector <uint8_t> data){
    this->messageID = (data[1] << 8) + data[2];
    this->success = data[3];
    // uint16_t Ref_messageID = (data[4] << 8) + data[5];
    this->content = std::string(data.begin() + 6, data.end());
}


ConfirmPacket::ConfirmPacket(const std::vector<uint8_t> data){
    this->refID = std::to_string((data[1] << 8) + data[2]);
}

//TODO check this, if the serialize is not the same as in the example
ConfirmPacket::ConfirmPacket(uint16_t messageID){
    this->refID = std::to_string(messageID);
}

std::string ConfirmPacket::serialize(Connection&) {
    std::vector <uint8_t> packet;
    packet.push_back(0x00);
    uint16_t id_num = std::stoi(refID);

    packet.push_back(id_num >> 8);  // High byte
    packet.push_back(id_num & 0xFF);  // Low byte

    std::string packet_str(packet.begin(), packet.end());
    return packet_str;    
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
            std::cerr << "Invalid format of reply" << std::endl;
            exit(1);
        } 

        if(data[1] == "OK"){
            this->success = true;
        } else if(data[1] == "NOK"){
            this->success = false;
        } else {
            std::cerr << "ERR: Invalid format of reply" << std::endl;
            exit(1);
        }

        std::string content = "";
        for (std::vector<std::string>::size_type i = 3; i < data.size(); ++i) {
            content += data[i];
            if (i != data.size() - 1) {
               content += " ";
            }
        }
        this->content = content;
    }


std::vector<std::string>ReplyPacket::getData() {
    std::vector<std::string> data;
    data.push_back(std::to_string(success));
    data.push_back(content);
    data.push_back(std::to_string(messageID));

    return data;
}


std::vector<std::string> ErrorPacket::getData() {
    std::vector<std::string> data;
    data.push_back(dname);
    data.push_back(content);
    data.push_back(std::to_string(messageID));
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
                if(tokens.size() < 4){
                    //TODO throw exception
                    std::cout << "Invalid number of arguments" << std::endl;
                    exit(1);
                }
                ReplyPacket reply_packet(tokens);
                return reply_packet;
            }
            else if(packetType == "ERR"){
                if(tokens.size() < 4){
                    //TODO throw exception
                    std::cout << "Invalid number of arguments" << std::endl;
                    exit(1);
                }
                ErrorPacket error_packet(tokens);
                return error_packet;
            }
            else if(packetType == "MSG"){
                //TODO FIX
                if(tokens.size() < 5){
                    //TODO throw exception
                    std::cout << "Invalid number of arguments" << std::endl;
                    exit(1);
                }
                // i want every token from 4 to the end
                std::string content = "";
                for (std::vector<std::string>::size_type i = 4; i < tokens.size(); ++i) {
                    content += tokens[i];
                    if (i != tokens.size() - 1) {
                        content += " ";
                    }
                }
                MsgPacket msgPacket(tokens[2], content);
                return msgPacket;
            }
            else if(packetType == "BYE"){
                //TODO throw exception
                if(tokens.size() < 1){
                    std::cout << "Invalid number of arguments" << std::endl;
                    exit(1);
                }
                std::cout << "Received BYE packet, ending connection" << std::endl;
                exit(1);
            }
            else{
                std::cerr << "ERR: Invalid packet" << std::endl;
                return NullPacket();                            
            }
        }
        //TODO throw exception
        return NullPacket();    
    }
    else if(connection.protocol == Connection::Protocol::UDP){
        // Convert data to vector of bytes
        std::vector<uint8_t> data_bytes(data.begin(), data.end());
        uint8_t first_byte = data_bytes[0];
        switch(first_byte){
            case 0x00:{
                ConfirmPacket confirm_packet(data_bytes);
                return confirm_packet;
                break;
                }
            case 0x01:{
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
            case 0xF:{
                std::cout << "Received BYE packet, ending connection" << std::endl;
                exit(1);
            }

            default:
                return NullPacket();
                
        }

    }
    std::cerr<< "ERR: Invalid protocol" << std::endl;
    exit(1);
}



std::string ByePacket::serialize(Connection &connection) {
    if(connection.protocol == Connection::Protocol::TCP){
        return "BYE" + CRLF;
    }
    else{
        std::vector<uint8_t> packet;
        //first byte is 0xFF, next 2 bytes are message ID
        packet.push_back(0xFF);
        uint16_t id_num = connection.message_id++;
        packet.push_back(id_num >> 8);  // High byte
        packet.push_back(id_num & 0xFF);  // Low byte

        std::string packet_str(packet.begin(), packet.end());
        return packet_str;
    }
}


// int main() {
//     std::string test = "Test string i want to serialize";
//     std::string dname = "Matej";

//     MsgPacket msg(dname, test);
//     std::cout << msg.serialize();

//     return 0;
// }