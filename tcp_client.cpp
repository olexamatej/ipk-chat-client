#include "tcp_client.h"


TCPClient::TCPClient(std::string ip_address, std::string port) {
    this->ip_address = ip_address;
    this->port = port;
    connect();
}

TCPClient::~TCPClient() {
    close(_socket);
}


void TCPClient::connect() {
    int status;
    struct addrinfo hints;
    struct addrinfo *servinfo;
    std::memset(&hints, 0, sizeof hints);
    
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
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
    
    if (::connect(_socket, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
        close(_socket);
        perror("connect");
        exit(1);
    }

    freeaddrinfo(servinfo);
}

void TCPClient::send(std::string message) {
    ssize_t bytes_sent = ::send(_socket, message.c_str(), message.size(), 0);
    if (bytes_sent == -1) {
        perror("send");
    }
    // std::cout << "Sent " << bytes_sent << " bytes to " << ip_address << ":" << port << std::endl;
}

std::string TCPClient::receive(){
    char buffer[1024];
    ssize_t bytes_received = recv(_socket, buffer, sizeof(buffer), 0);
    if (bytes_received == -1) {
        perror("recv");
        return "";
    } else if (bytes_received == 0) {
        // std::cout << "Connection closed by " << ip_address << ":" << port << std::endl;
        return "";
    }
    // std::cout << "Received " << bytes_received << " bytes from " << ip_address << ":" << port << std::endl;
    return std::string(buffer, bytes_received);
}


// int main() {
//     std::string ip_address = "127.0.0.1";
//     std::string port = "5553";
    
//     // TCPClient client(ip_address, port);
//     // client.send("Mam rad vlaky\n");
//     TCPClient client(ip_address, port);
//     std::cout << "conntected \n";
//    while(1) {
//         std::string message = client.receive();
//         if (message.empty()) {
//             break;
//         }
//         std::cout << message << std::endl;
//     }

//     return 0;
// }
