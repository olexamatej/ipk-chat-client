#include "argumentParser.h"
#include <iostream>
#include <stdlib.h>

void ArgumentParser::parse(){
    for(int i = 0; i < this->argc; i++){
        switch(this->argv[i][1]){
            case 't':
                if(std::string(this->argv[i+1]) == "tcp"){
                    i++;
                    this->protocol = Connection::Protocol::TCP;
                }
                else if(std::string(this->argv[i+1]) == "udp"){
                    i++;
                    this->protocol = Connection::Protocol::UDP;
                }
                else{
                    printf("Invalid protocol\n");
                    exit(1);
                }
                break;
            case 's':
                this->server_ip = this->argv[i+1];
                i++;
                break;
            case 'p':
                this->port = atoi(this->argv[i+1]);
                i++;
                break;
            case 'r':
                this->retries = atoi(this->argv[i+1]);
                i++;
                break;
            case 'd':
                this->timeout = atoi(this->argv[i+1]);
                i++;
                break;    
            case 'h':
                printf("Usage: ./runner -t [tcp/udp] -s [server ip] -p [port]\n");
                exit(0);
            break; 
            default:
                break;
        }
    }
}