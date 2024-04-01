#ifndef CLIENT_H
#define CLIENT_H
#include <string>

// Abstract class for the client
class Client {
public:
    virtual ~Client();
    virtual void send(std::string) = 0;
    virtual std::string receive() = 0;
    virtual void connect() = 0;
};
#endif