#ifndef packet_H
#define packet_H

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <variant>
#include <string>
#include "connection.h"

// Define the types of packets that can be received
#define RECV_PACKET_TYPE MsgPacket, ErrorPacket, ReplyPacket, ConfirmPacket, ByePacket, NullPacket

// Abstract class for the packet
class Packet {
    public:
        virtual std::string serialize(Connection&){return "";};
    protected:
        std::string input;
};

// Class for packet of message type
class MsgPacket : public Packet {
    public:
    //constructor for different purposes
        MsgPacket(const std::string& dname, const std::string& content);
        MsgPacket(const std::vector<uint8_t> data);
        using Packet::serialize;
        std::string serialize(Connection &connection);
        std::vector <std::string> getData();
        bool LegalCheck();
    protected:
        std::string dname;
        std::string content;
        uint16_t messageID;
};

//Class for packet of join type
class JoinPacket : public Packet {
    public:
        JoinPacket(const std::vector<std::string>& arguments, const std::string& dname);
        std::string serialize(Connection &connection);
        bool LegalCheck();
    protected:
        std::string dname;
        std::string id;
};

//Class for packet of auth type
class AuthPacket : public Packet {
    public:
        AuthPacket(const std::vector<std::string>& arguments);
        std::string serialize(Connection &connection);
        std::vector<std::string> getData();
        std::string id;
        std::string dname;
        std::string secret;
        bool LegalCheck();
};
//Class for packet of Error type

class ErrorPacket : public Packet {
    public:
        ErrorPacket(const std::vector<std::string> data);
        ErrorPacket(const std::vector<uint8_t> data);
        ErrorPacket(const std::string content, std::string dname);
        std::vector <std::string> getData();
        std::string serialize(Connection &connection);
    protected:
        std::string dname;
        std::string content;
        uint16_t messageID;
};

//Class for packet of Reply type

class ReplyPacket : public Packet {
    public:
        ReplyPacket(const std::vector<std::string> data);
        ReplyPacket(const std::vector<uint8_t> data);
        std::vector <std::string> getData();
    protected:
        bool success;
        std::string content;
        uint16_t messageID;
};

//Class for packet of Confirm type
class ConfirmPacket : public Packet {
    public:
        ConfirmPacket(const std::vector<uint8_t> data);
        ConfirmPacket(uint16_t messageID);
        std::vector <std::string> getData();
        std::string serialize(Connection&);
    protected:
        std::string refID;
};

//Class for packet of Null type
class NullPacket : public Packet {
    public:
        std::string serialize(Connection&){return "";};
        std::vector <std::string> getData() {return {};}
        bool rename = false;
};

//Class for packet of Bye type

class ByePacket : public Packet {
    public:
        std::string serialize(Connection &connection);
        std::vector <std::string> getData() {
            //return this->messageID in a vector of strings
            return {std::to_string(this->messageID)};
        }
    protected:
        uint16_t messageID;
};

std::variant<RECV_PACKET_TYPE> ReceiveParser(const std::string data, Connection &connection);


#endif // packet_H
