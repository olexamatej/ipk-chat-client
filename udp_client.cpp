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
    this->_socket = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if (_socket == -1) {
        perror("socket");
        exit(1);
    }
    
    //TODO FREE memory at the end
    // freeaddrinfo(servinfo);
}

void UDPClient::send(std::string message) {
    ssize_t bytes_sent = ::sendto(_socket, message.c_str(), message.size(), 0, this->servinfo->ai_addr, this->servinfo->ai_addrlen);
    if (bytes_sent == -1) {
        perror("send");
    }
    std::cout << "Sent " << bytes_sent << " bytes to " << ip_address << ":" << port << std::endl;
}

std::string UDPClient::receive() {
    //TODO change buffer size
    char buffer[1024]; // Declare the buffer variable
    ssize_t bytes_received = recv(_socket, buffer, sizeof(buffer), 0);
    if (bytes_received == -1) {
        perror("recv");
    }
    std::cout << "Received " << bytes_received << " bytes from " << ip_address << ":" << port << std::endl;
    return std::string(buffer, bytes_received);
}

// int main() {

//     std::string ip_address = "127.0.0.1";
//     std::string port = "4567";
    
//     UDPClient client(ip_address, port);
//     client.send("Mam rad vlaky\n");
//     std::cout << client.receive() << std::endl;
//     return 0;
// }
