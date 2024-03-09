#include "connection.h"

Connection::Connection(std::string ip_address, std::string port){
    this->ip_address = ip_address;
    this->port = port;
    this->display_name = "";
    this->id = "";
    this->secret = "";
}

void Connection::clearAfterAuth(){
    this->id = "";
    this->secret = "";
    this->display_name = "";
}
