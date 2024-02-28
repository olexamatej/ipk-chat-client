#include "udp_client.h"


UDPClient::UDPClient(std::string ip_address, std::string port){
    this->ip_address = ip_address;
    this->port = port;
    create_socket();
}

void UDPClient::create_socket() {
    int status;
    struct addrinfo hints;
    
    std::memset(&hints, 0, sizeof hints);
    
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = 0;
    
    if ((status = getaddrinfo(ip_address.c_str(), port.c_str(), &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    _socket = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if (_socket == -1) {
        perror("socket");
        exit(1);
    }
  
    freeaddrinfo(servinfo);
}

void UDPClient::send(std::string message) {
    ssize_t bytes_sent = ::sendto(_socket, message.c_str(), message.size(), 0, this->servinfo->ai_addr, this->servinfo->ai_addrlen);
    if (bytes_sent == -1) {
        perror("send");
    }
    std::cout << "Sent " << bytes_sent << " bytes to " << ip_address << ":" << port << std::endl;
}

int main() {

    std::string ip_address = "127.0.0.1";
    std::string port = "5553";
    
    UDPClient client(ip_address, port);
    client.send("Mam rad vlaky\n");
    
    return 0;
}
