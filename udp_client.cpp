#include "udp_client.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

UDPClient::UDPClient(){
}

UDPClient::UDPClient(std::string ip_address, std::string port){
    this->ip_address = ip_address;
    this->port = port;
    connect();
}
//creating socket

void UDPClient::connect() {
    //resolve addrinfo
    struct addrinfo *server_info;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       //ipv4
    hints.ai_socktype = SOCK_DGRAM;  //udp
    hints.ai_protocol = 0;           // Protocol

    int status = getaddrinfo(ip_address.c_str(), port.c_str(), &hints, &server_info);

    if (status != 0 || server_info->ai_addr == NULL)
    {
        std::cerr << "Failed to resolve hostname.\n";
        exit(1);
    }

    //initialize socket
    if ((this->_socket = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol)) < 0)
    {
        std::cerr << "Invalid socket.\n";
        exit(1);
    }

    struct timeval tv;
    tv.tv_sec = 1; // Timeout in seconds
    tv.tv_usec = 0;
    if (setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)) < 0) {
        perror("setsockopt");
    }

    sockaddr_in sendAddr;

    //resolving UDP dynamic port
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)server_info->ai_addr;
    inet_pton(AF_INET, ip_address.c_str(), &(ipv4->sin_addr)); 

    sendAddr.sin_family = AF_INET;
    sendAddr.sin_addr.s_addr = ipv4->sin_addr.s_addr;
    sendAddr.sin_port = htons(std::stoi(port));



    this->serverAddr = sendAddr;

    sockaddr_in recvAddress;
    recvAddress.sin_family = AF_INET;
    recvAddress.sin_addr.s_addr = INADDR_ANY;
    recvAddress.sin_port = 0; 

    // Bind the socket to the local address and port
    if (bind(this->_socket, (const sockaddr*)&recvAddress, sizeof(sockaddr_in)) < 0) {
        std::cerr << "Failed to bind socket." << std::endl;
        return;
    }
}

void UDPClient::send(std::string message) {
    //print ip address
    ssize_t bytes_sent = sendto(this->_socket, message.c_str(), message.size(), 0, (struct sockaddr*)&this->serverAddr, sizeof(this->serverAddr));
    if (bytes_sent == -1) {
        perror("send");
    }
}

std::string UDPClient::receive() {
    char buffer[1024];
    sockaddr_in senderAddr;
    socklen_t senderAddrLen = sizeof(senderAddr);

     // Set up a timeout for recvfrom


    ssize_t bytes_received = recvfrom(_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&senderAddr, &senderAddrLen);
    if (bytes_received == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // Timeout occurred
            return ""; // Return empty string to indicate no message received within the timeout
        } else {
            perror("recv");
            return ""; // Return empty string upon error
        }
    }

    // Update sendAddr with sender's address and port
    this->serverAddr = senderAddr;

    return std::string(buffer, bytes_received);
}

