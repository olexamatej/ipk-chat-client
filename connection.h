#ifndef CONNECTION_H
#define CONNECTION_H
#include <string>
#include <iostream>



class Connection {
    public:

        enum Protocol {
            TCP,
            UDP
        };

        std::string ip_address;        
        std::string port;
        std::string display_name;
        std::string id;
        std::string secret;
        uint16_t message_id;
        Protocol protocol;
        Connection(std::string ip_address, std::string port, Protocol protocol);
        void clearAfterAuth();
};



#endif