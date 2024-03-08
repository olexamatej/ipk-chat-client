#ifndef PACKET_TCP_H
#define PACKET_TCP_H

#include <iostream>
#include <string>
#include <vector>


class PacketTCP {
    public:
        virtual std::string serialize() = 0;
    protected:
        std::string input;
};

class MsgPacket : public PacketTCP {
    public:
        MsgPacket(const std::string& dname, const std::string& content);
        std::string serialize();
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
        ErrorPacket(const std::vector<std::string>& arguments);
        std::string serialize();
    protected:
        std::string dname;
        std::string content;
};




#endif // PACKET_TCP_H
