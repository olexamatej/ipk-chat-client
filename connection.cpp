#include "connection.h"

Connection::Connection(std::string ip_address, std::string port, Protocol protocol){
    this->ip_address = ip_address;
    this->port = port;
    this->display_name = "";
    this->id = "";
    this->secret = "";
    this->protocol = protocol;
    this->message_id = 0;
    this->timeout = 250;
    this->retries = 3;
}

void Connection::clearAfterAuth(){
    this->id = "";
    this->secret = "";
    this->display_name = "";
}
