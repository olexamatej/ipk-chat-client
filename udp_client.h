#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H

#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>


class UDPClient {
    public:
        UDPClient(std::string ip_address, std::string port);
        void send(std::string message);
        std::string receive();
        void create_socket();
    private:
        int _socket;
        std::string ip_address;
        std::string port;
        struct addrinfo *servinfo;
};


#endif