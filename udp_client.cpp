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
    if((this->_socket = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol)) < 0)
    {
        std::cerr << "Invalid socket.\n";
        exit(1);
    }    

    sockaddr_in sendAddr;
    sendAddr.sin_family = AF_INET;
    sendAddr.sin_addr.s_addr = inet_addr(ip_address.c_str());
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

    ssize_t bytes_received = recvfrom(_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&senderAddr, &senderAddrLen);
    if (bytes_received == -1) {
        perror("recv");
        return ""; // Return empty string upon error
    }

    // Update sendAddr with sender's address and port
    this->serverAddr = senderAddr;

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
