#include "tcp_client.h"
#include "inputParser.h"
#include "packet_tcp.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <vector>

#include <queue>
#include <condition_variable>


int main() {
    std::string ip_address = "127.0.0.1";
    std::string port = "5553";
    
    TCPClient client(ip_address, port);
    std::cout << "Connected \n";

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
            input_packet_queue.push(userInput.parseInput());
            queue_cond_var.notify_one(); 

        }
    });

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

            std::variant<PACKET_TYPE> packet = input_packet_queue.front();
            std::string serialized_packet = "";
            std::visit([&](auto& p) { serialized_packet = p.serialize(); }, packet);
            
            std::cout << serialized_packet << std::endl;

            input_packet_queue.pop();
            lock.unlock();

            client.send(serialized_packet);

        }
    });
    
    std::jthread receiveThread([&]() {
        while(1) {
            std::string message = client.receive();
            if (message.empty()) {
                break;
            }
            std::cout << message << std::endl;
        }
    });

    

    return 0;
}