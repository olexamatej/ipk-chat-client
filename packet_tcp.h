#ifndef PACKET_TCP_H
#define PACKET_TCP_H

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <variant>
#include <string>

#define RECV_PACKET_TYPE MsgPacket, ErrorPacket, ReplyPacket


class PacketTCP {
    public:
        virtual std::string serialize(){return "";};
    protected:
        std::string input;
};

class MsgPacket : public PacketTCP {
    public:
        MsgPacket(const std::string& dname, const std::string& content);
        std::string serialize();
        std::vector <std::string> getData(){return {dname, content};};
    protected:
        std::string dname;
        std::string content;
};

class JoinPacket : public PacketTCP {
    public:
        JoinPacket(const std::vector<std::string>& arguments, const std::string& dname);
        std::string serialize();
    protected:
        std::string dname;
        std::string id;
};


class AuthPacket : public PacketTCP {
    public:
        AuthPacket(const std::vector<std::string>& arguments);
        std::string serialize();
        std::vector<std::string> getData();
        std::string id;
        std::string dname;
        std::string secret;
};

class ErrorPacket : public PacketTCP {
    public:
        ErrorPacket(const std::vector<std::string> data);
        std::vector <std::string> getData();
    protected:
        std::string dname;
        std::string content;
};

class ReplyPacket : public PacketTCP {
    public:
        ReplyPacket(const std::vector<std::string> data);
        std::vector <std::string> getData();
    protected:
        bool success;
        std::string content;
};

class NullPacket : public PacketTCP {
    public:
        std::string serialize(){return "";};
};

class ByePacket : public PacketTCP {
    public:
        std::string serialize();
};

std::variant<RECV_PACKET_TYPE> ReceiveParser(const std::string data);


#endif // PACKET_TCP_H
