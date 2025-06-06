#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include "client.h"

class TCPClient : public Client{
    public:
        TCPClient(std::string ip_address, std::string port);
        ~TCPClient();
        void send(std::string message);
        void connect();
        std::string receive();
        void receiveTimeout(int){};
    private:
        int _socket;
        std::string ip_address;
        std::string port;
};


#endif