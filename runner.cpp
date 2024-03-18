#include "runner.h"
#define TIMEOUT 50
#include <signal.h>

void handle_sigint(int sig) {
    ByePacket byePacket;

    std::string serialized_byePacket = byePacket.serialize();

    // client.send(serialized_byePacket);

    exit(0);
}

// Runner::Runner(std::string ip_address, std::string port, Connection::Protocol protocol) {
//     this->ip_address = ip_address;
//     this->port = port;
//     this->protocol = protocol;
//     client = UDPClient(ip_address, port); // Fix: Initialize the client object using the constructor syntax
// }


void Runner::inputScanner(Connection &connection){
    std::string line;
    while (std::getline(std::cin, line)) {
        Input userInput;
        userInput.getNewInput(line);
        std::lock_guard<std::mutex> lock(queue_mutex);
        input_packet_queue.push(userInput.parseInput(connection));
        queue_cond_var.notify_one(); 
    }

}

void Runner::packetSender(Connection &connection) {
        while(1) {
            std::unique_lock<std::mutex> lock(queue_mutex);
            queue_cond_var.wait(lock, [&] { return !input_packet_queue.empty(); });
            if(input_packet_queue.empty()){
                continue;
            }

            //get the first packet from the queue and call .serialize
            send_packet = input_packet_queue.front();

            std::string serialized_packet = "";
            std::visit([&](auto& p) { serialized_packet = p.serialize(connection); }, send_packet);

            input_packet_queue.pop();
            lock.unlock();

            client.send(serialized_packet);

            std::cout << "Waiting\n";
            //give this mutex a timeout of 5 seconds

            std::unique_lock<std::mutex> reply_lock(reply_mutex);
            if(reply_cond_var.wait_for(reply_lock, std::chrono::seconds(TIMEOUT)) == std::cv_status::timeout) {
                std::cout << "Timeout\n";
            } else {
                std::cout << "Good\n";
                std::cout << "Message sent: " << serialized_packet << std::endl; // Print the message that was sent
            }
        }
    }


void Runner::packetReceiver(Connection &connection) {
    while (1) {
        std::string reply = client.receive();

        std::variant<RECV_PACKET_TYPE> recv_packet = ReceiveParser(reply, connection);
        
        if (std::holds_alternative<AuthPacket>(send_packet) || std::holds_alternative<JoinPacket>(send_packet)) {
            processAuthJoin(connection, reply, recv_packet);
        }
        else if(std::holds_alternative<ConfirmPacket>(recv_packet)){
            std::cout << "prisiel confirm\n";
        }
        else{
            //call the .getData() method of the packet and print the data
            std::cout << "Message received: \n";
            std::visit([&](auto& p) { std::cout << "Message received: " << p.getData()[1] << std::endl; }, recv_packet);
            uint16_t messageID = 0;
            ConfirmPacket confirm_packet(messageID);
            client.send(confirm_packet.serialize());
        }
        
        std::unique_lock<std::mutex> lock(reply_mutex);
        reply_cond_var.notify_one();
    }
}

void Runner::processAuthJoin(Connection &connection, std::string &reply, std::variant<RECV_PACKET_TYPE> recv_packet) {

    if (std::holds_alternative<ReplyPacket>(recv_packet)) {
        ReplyPacket reply_packet = std::get<ReplyPacket>(recv_packet);
        reply = client.receive();
        // write reply.getData()[1]

        std::visit([&](auto& p) { std::cout << p.getData()[1] << std::endl; }, recv_packet);

        std::variant<RECV_PACKET_TYPE> recv_packet = ReceiveParser(reply, connection);
        
        if (std::holds_alternative<ConfirmPacket>(recv_packet)) {
            ConfirmPacket confirm_packet = std::get<ConfirmPacket>(recv_packet);
            std::vector<std::string> packet_data = confirm_packet.getData();
            client.send(reply);
        }
    }

    else if (std::holds_alternative<ConfirmPacket>(recv_packet)) {
        ConfirmPacket confirm_packet = std::get<ConfirmPacket>(recv_packet);
        std::string prev_reply = reply;
        reply = client.receive();
        std::variant<RECV_PACKET_TYPE> recv_packet = ReceiveParser(reply, connection);
        
        if (std::holds_alternative<ReplyPacket>(recv_packet)) {
            ReplyPacket reply_packet = std::get<ReplyPacket>(recv_packet);
            std::vector<std::string> packet_data = reply_packet.getData();
            std::cout << packet_data[1] << std::endl;
            client.send(prev_reply);
        }
    }
}


void Runner::run(){
    Connection connection = Connection(ip_address, port, Connection::Protocol::UDP);

    std::cout << "Connected \n";
    
    // signal(SIGINT, handle_sigint);

     //thread for reading stdin, parsing them and sending them to other thread
     std::jthread inputThread([&](){
        inputScanner(connection);
    });

    //thread for sending messages to server
    std::jthread sendThread([&]() {
        //while first thread is running, this thread will be waiting for input
        packetSender(connection);
    });
    
    std::jthread receiveThread([&]() {
        packetReceiver(connection);
    });
}