#ifndef CONNECTION_H
#define CONNECTION_H
#include <string>
#include <iostream>
#include <set>
#include <map>
#include <cstdint>
#include <atomic>


// Class to store the connection details
class Connection {
    public:

        Connection(){};
        // Enum for the protocol
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
        int timeout;
        int retries;
        Protocol protocol;
        // Constructor to set the connection details
        Connection(std::string ip_address, std::string port, Protocol protocol);
        // Function to clean auth data
        void clearAfterAuth();
        std::atomic<int> exit_flag{-1};
};



#endif