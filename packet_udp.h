#ifndef PACKET_UDP_H
#define PACKET_UDP_H

#include <string>
#include <cstdint>
#include <vector>
#include <iostream>

class PacketUDP {
    public:
        virtual std::string serialize(){return "";};
    protected:
        std::string input;
};

class AuthPacket : PacketUDP{
    public:
        AuthPacket(const std::string& id, const std::string& dname, const std::string& secret);
        std::string serialize();
    private:
        std::string id;
        std::string dname;
        std::string secret;
};

class JoinPacket : PacketUDP{
    public:
        JoinPacket(const std::vector<std::string>& arguments, const std::string& display_name);
        std::string serialize();
    private:
        std::string display_name;
        std::string id;
};

class MessagePacket : PacketUDP{
    public:
        MessagePacket(const std::vector<std::string>& arguments, const std::string& display_name);
        std::string serialize();
    private:
        std::string display_name;
        std::string id;
        std::string message;
};

class ErrorPacket : PacketUDP{
    public:
        ErrorPacket(const std::vector<std::string>& arguments);
        std::string serialize();
    private:
        std::string id;
        std::string display_name;
        std::string message;
};

class ReplyPacket : PacketUDP{
    public:
        ReplyPacket(const std::vector<std::string>& arguments);
        std::string serialize();
    private:
        std::string id;
        std::string display_name;
        std::string message;
};


#endif