#ifndef CLIENT_H
#define CLIENT_H
#include <string>

class Client {
public:
    virtual ~Client();
    virtual void send(std::string message) = 0;
    virtual std::string receive() = 0;
    virtual void connect() = 0;
};
#endif