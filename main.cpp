#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>



int main() {

    std::string ip_address = "127.0.0.1";
    std::string port = "5553";
    std::string message = "Hello, World!";

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

    int _socket = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if (_socket == -1) {
        perror("socket");
        exit(1);
    }

    if (connect(_socket, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
        close(_socket);
        perror("connect");
        exit(1);
    }

    // Send the message using send
    ssize_t bytes_sent = send(_socket, message.c_str(), message.size(), 0);
    if (bytes_sent == -1) {
    perror("send");
    }
    std::cout << "Sent " << bytes_sent << " bytes to " << ip_address << ":" << port << std::endl;
    freeaddrinfo(servinfo);
    close(_socket);
    return 0;
}
