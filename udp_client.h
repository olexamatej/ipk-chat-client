#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H

#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "client.h"
class UDPClient : public Client{
    public:
        UDPClient();
        UDPClient(const std::string ip_address,const std::string port);
        void receiveTimeout(int milliseconds);
        void send(std::string message);
        std::string receive();
        void connect();
    private:
        int _socket;
        std::string ip_address;
        std::string port;
        sockaddr_in serverAddr;

};


#endif