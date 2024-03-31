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


class Packet {
    public:
        virtual std::string serialize(Connection&){return "";};
    protected:
        std::string input;
};

class MsgPacket : public Packet {
    public:
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

class JoinPacket : public Packet {
    public:
        JoinPacket(const std::vector<std::string>& arguments, const std::string& dname);
        std::string serialize(Connection &connection);
        bool LegalCheck();
    protected:
        std::string dname;
        std::string id;
};


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

class ConfirmPacket : public Packet {
    public:
        ConfirmPacket(const std::vector<uint8_t> data);
        ConfirmPacket(uint16_t messageID);
        std::vector <std::string> getData();
        std::string serialize(Connection&);
    protected:
        std::string refID;
};

class NullPacket : public Packet {
    public:
        std::string serialize(Connection&){return "";};
        std::vector <std::string> getData() {return {};}
        bool rename = false;
};

class ByePacket : public Packet {
    public:
        std::string serialize(Connection &connection);
        std::vector <std::string> getData() {return {};}
};

std::variant<RECV_PACKET_TYPE> ReceiveParser(const std::string data, Connection &connection);


#endif // packet_H
