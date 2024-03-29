#ifndef packet_H
#define packet_H

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <variant>
#include <string>
#include "connection.h"

#define RECV_PACKET_TYPE MsgPacket, ErrorPacket, ReplyPacket, ConfirmPacket, ByePacket, NullPacket


class PacketTCP {
    public:
        virtual std::string serialize(){return "";};
    protected:
        std::string input;
};

class MsgPacket : public PacketTCP {
    public:
        MsgPacket(const std::string& dname, const std::string& content);
        MsgPacket(const std::vector<uint8_t> data);
        std::string serialize(Connection &connection);
        std::vector <std::string> getData();
        bool LegalCheck();
    protected:
        std::string dname;
        std::string content;
        uint16_t messageID;
};

class JoinPacket : public PacketTCP {
    public:
        JoinPacket(const std::vector<std::string>& arguments, const std::string& dname);
        std::string serialize(Connection &connection);
        bool LegalCheck();
    protected:
        std::string dname;
        std::string id;
};


class AuthPacket : public PacketTCP {
    public:
        AuthPacket(const std::vector<std::string>& arguments);
        std::string serialize(Connection &connection);
        std::vector<std::string> getData();
        std::string id;
        std::string dname;
        std::string secret;
        bool LegalCheck();
};

class ErrorPacket : public PacketTCP {
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

class ReplyPacket : public PacketTCP {
    public:
        ReplyPacket(const std::vector<std::string> data);
        ReplyPacket(const std::vector<uint8_t> data);
        std::vector <std::string> getData();
    protected:
        bool success;
        std::string content;
        uint16_t messageID;
};

class ConfirmPacket : public PacketTCP {
    public:
        ConfirmPacket(const std::vector<uint8_t> data);
        ConfirmPacket(uint16_t messageID);
        std::vector <std::string> getData();
        std::string serialize();
    protected:
        std::string refID;
};

class NullPacket : public PacketTCP {
    public:
        std::string serialize(Connection &connection){return "";};
        std::vector <std::string> getData() {return {};}
        bool rename = false;
};

class ByePacket : public PacketTCP {
    public:
        std::string serialize(Connection &connection);
        std::vector <std::string> getData() {return {};}
};

std::variant<RECV_PACKET_TYPE> ReceiveParser(const std::string data, Connection &connection);


#endif // packet_H
