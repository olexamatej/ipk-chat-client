#include "tcp_client.h"
#include "inputParser.h"
#include "packet.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <vector>
#include <signal.h>
#include <queue>
#include <condition_variable>

//TODO
#define TIMEOUT 50



std::string ip_address = "127.0.0.1";
std::string port = "4567";
    
TCPClient client(ip_address, port);


void handle_sigint(int sig) {
    ByePacket byePacket;

    std::string serialized_byePacket = byePacket.serialize();

    client.send(serialized_byePacket);

    exit(0);
}

/*
int main() {
  
    std::cout << "Connected \n";
    
    signal(SIGINT, handle_sigint);
    Connection connection = Connection(ip_address, port, Connection::Protocol::TCP);

    std::queue<std::variant<PACKET_TYPE>> input_packet_queue;

    std::mutex queue_mutex;
    std::condition_variable queue_cond_var;

     //thread for reading stdin, parsing them and sending them to other thread
     std::jthread inputThread([&](){
        std::string line;
        while (std::getline(std::cin, line)) {

            Input userInput;
            userInput.getNewInput(line);

            std::lock_guard<std::mutex> lock(queue_mutex);
            input_packet_queue.push(userInput.parseInput(connection));
            queue_cond_var.notify_one(); 

        }
    });

    std::condition_variable reply_cond_var;
    std::mutex reply_mutex;
    std::variant<PACKET_TYPE> send_packet = NullPacket();

    //thread for sending messages to server
    std::jthread sendThread([&]() {
        //while first thread is running, this thread will be waiting for input
        while (true) {
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

            if(std::holds_alternative<AuthPacket>(send_packet) || std::holds_alternative<JoinPacket>(send_packet)){
                std::cout << "Waiting\n";
                //give this mutex a timeout of 5 seconds

                std::unique_lock<std::mutex> lock(reply_mutex);
                if(reply_cond_var.wait_for(lock, std::chrono::seconds(TIMEOUT)) == std::cv_status::timeout) {
                    std::cout << "Timeout\n";
                } else {
                    std::cout << "Message sent: " << serialized_packet << std::endl; // Print the message that was sent
                }   
            }
        }
    });
    
    std::jthread receiveThread([&]() {
        while(1) {
            std::string reply = client.receive();
            std::cout << "Received: ";
            std::cout << reply << std::endl;            
            
            //print message that was sent
            
            std::variant<RECV_PACKET_TYPE> recv_packet = ReceiveParser(reply, connection);
            std::vector<std::string> packet_data;
            if(std::holds_alternative<ReplyPacket>(recv_packet)){
                std::visit([&](auto& p) { packet_data = p.getData(); }, recv_packet);
                std::cout << packet_data[0] << std::endl;
                if(packet_data[0] == "1"){
                    std::cout << "Success\n";
                } else {
                    std::cout << "Failure\n";
                    if(std::holds_alternative<AuthPacket>(send_packet)){
                        std::cout << "Authentication failed\n";
                        connection.clearAfterAuth(); 
                    }
                }
            }
            
            std::unique_lock<std::mutex> lock(reply_mutex);
            reply_cond_var.notify_one();
        }
    });



    return 0;
}