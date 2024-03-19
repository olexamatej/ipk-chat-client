#ifndef ARGUMENTPARSER_H
#define ARGUMENTPARSER_H

#include "connection.h"
class ArgumentParser{
    public:
        ArgumentParser(int argc, char *argv[]){
            this->argc = argc;
            this->argv = argv;
            this->timeout = 250;
            this->retries = 3;
            this->port = 4567;
            parse();
        }
        void parse();
        Connection::Protocol protocol;
        std::string server_ip;
        uint16_t port;
        uint16_t timeout;
        uint8_t retries;

    private:
        int argc;
        char **argv;
};

#endif