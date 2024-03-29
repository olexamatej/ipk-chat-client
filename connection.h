#ifndef CONNECTION_H
#define CONNECTION_H
#include <string>
#include <iostream>
#include <set>
#include <map>
#include <cstdint>


class Connection {
    public:

        enum Protocol {
            TCP,
            UDP
        };
        std::map<uint16_t, bool> message_id_map;
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