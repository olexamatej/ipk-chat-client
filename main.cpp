#include "tcp_client.h"
#include "inputParser.h"
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

    std::queue<std::string> inputQueue;
    std::mutex queueMutex;
    std::condition_variable queueCondVar;

     //thread for reading stdin, parsing them and sending them to other thread
     //thread for reading stdin, parsing them and sending them to other thread
     std::jthread inputThread([&](){
        std::string line;
        while (std::getline(std::cin, line)) {

            Input userInput;
            userInput.getNewInput(line);

            std::lock_guard<std::mutex> lock(queueMutex);
            inputQueue.push(userInput.parseInput());
            queueCondVar.notify_one(); 

        }
    });

    //thread for sending messages to server
    std::jthread sendThread([&]() {
        //while first thread is running, this thread will be waiting for input
        while (true) {
            std::unique_lock<std::mutex> lock(queueMutex);

            queueCondVar.wait(lock, [&] { return !inputQueue.empty(); });
            if(inputQueue.empty()){
                continue;
            }

            std::string packet_to_send = inputQueue.front();
            inputQueue.pop();
            lock.unlock();

            std::string message = packet_to_send;
            client.send(message);

        }
    });
    
    // std::jthread receiveThread([&]() {
    //     while(1) {
    //         std::string message = client.receive();
    //         if (message.empty()) {
    //             break;
    //         }
    //         std::cout << message << std::endl;
    //     }
    // });

    

    return 0;
}