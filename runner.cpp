#include "runner.h"
#define TIMEOUT 50
#include <signal.h>


Runner::Runner(ArgumentParser argParser){
    this->ip_address = argParser.server_ip;
    this->port = std::to_string(argParser.port);
    this->protocol = argParser.protocol;
    this->timeout = argParser.timeout;
    this->retries = argParser.retries;

    if(this->protocol == Connection::Protocol::UDP){
        std::cout << "UDP\n";
        this->client = new UDPClient(this->ip_address,this->port);
    }
    else if(this->protocol == Connection::Protocol::TCP){
        this->client = new TCPClient(this->ip_address,this->port);
    }
    else {
        std::cout << "Invalid protocol\n";
        exit(1);
    }
}

Runner::~Runner(){
    delete client;
}

void handle_sigint(int sig) {
    ByePacket byePacket;

    std::string serialized_byePacket = byePacket.serialize();

    // client.send(serialized_byePacket);

    exit(0);
}


void Runner::inputScanner(Connection &connection){
    std::string line;
    if(connection.protocol == Connection::Protocol::UDP){
        while (std::getline(std::cin, line)) {
            Input userInput;
            userInput.getNewInput(line);
            
            std::lock_guard<std::mutex> lock(queue_mutex);
        
            input_packet_queue.push(userInput.parseInput(connection));
            //if userInput is nullpacket, then continue
            if(std::holds_alternative<NullPacket>(input_packet_queue.back())){
                std::cout << "Invalid input" << std::endl;
                input_packet_queue.pop();
                continue;
            }    
            connection.message_id_map[connection.message_id-1] = false;

            queue_cond_var.notify_one(); 
        }
    }
    else if(connection.protocol == Connection::Protocol::TCP){
         while (std::getline(std::cin, line)) {
            Input userInput;
            userInput.getNewInput(line);
            
            std::lock_guard<std::mutex> lock(queue_mutex);
        
            input_packet_queue.push(userInput.parseInput(connection));
            //if userInput is nullpacket, then continue
            if(std::holds_alternative<NullPacket>(input_packet_queue.back())){
                std::cout << "Invalid input" << std::endl;
                input_packet_queue.pop();
                continue;
            }    

            queue_cond_var.notify_one(); 
        }
    }
}


void Runner::packetSenderTCP(Connection &connection){
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
            

            client->send(serialized_packet);

            if(std::holds_alternative<AuthPacket>(send_packet) || std::holds_alternative<JoinPacket>(send_packet)){
                std::cout << "Waiting\n";
                //give this mutex a timeout of 5 seconds

                std::unique_lock<std::mutex> lock(reply_mutex);
                reply_cond_var.wait(lock);
                std::cout << "Good\n";
                std::cout << "Message sent: " << serialized_packet << std::endl; // Print the message that was sent
            }
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
            
        for (int attempt = 0; attempt < this->retries+1; ++attempt) {
                client->send(serialized_packet);

                std::cout << "Waiting\n";

                std::unique_lock<std::mutex> reply_lock(reply_mutex);
                if(reply_cond_var.wait_for(reply_lock, std::chrono::milliseconds(this->timeout)) == std::cv_status::timeout) {
                    std::cout << "Timeout\n";
                } else {
                    std::cout << "Good\n";
                    std::cout << "Message sent: " << serialized_packet << std::endl; // Print the message that was sent
                    break; // Break out of the loop if the message was sent successfully
                }
            }
        }
    }


void Runner::packetReceiverTCP(Connection &connection){
         while(1) {
            std::string reply = client->receive();
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
                std::cout << packet_data[1] << std::endl;
            }
            else if(std::holds_alternative<ErrorPacket>(recv_packet)){
                //send bye packet and end
                std::cout<< "eyo";
                ByePacket bye_packet;
                client->send(bye_packet.serialize());
                std::cout << "Error received, ending connection" << std::endl;
                exit(1);
            }
            else if(std::holds_alternative<NullPacket>(recv_packet)){
                //create error packet and send it
                std::cout << "posielam error\n";
                ErrorPacket error_packet("Invalid packet received", connection.display_name);
                client->send(error_packet.serialize(connection));
                std::cout << "Invalid packet received, ending connection" << std::endl;
                ByePacket bye_packet;
                client->send(bye_packet.serialize());
                exit(1);
            }
            else{
                std::cout << "Message received: \n";
                std::vector<std::string> packet_data = std::visit([&](auto& p) { return p.getData(); }, recv_packet);
                std::cout << packet_data[1] << std::endl;
            }
            
            std::unique_lock<std::mutex> lock(reply_mutex);
            reply_cond_var.notify_one();
        }
}


void Runner::packetReceiver(Connection &connection) {
    while (1) {
        std::string reply = client->receive();

        std::variant<RECV_PACKET_TYPE> recv_packet = ReceiveParser(reply, connection);
        
        if (std::holds_alternative<AuthPacket>(send_packet) || std::holds_alternative<JoinPacket>(send_packet)) {
            processAuthJoin(connection, reply, recv_packet);
        }
        else if(std::holds_alternative<ConfirmPacket>(recv_packet)){
            ConfirmPacket confirm_packet = std::get<ConfirmPacket>(recv_packet);
            std::vector<std::string> packet_data = confirm_packet.getData();
            uint16_t messageID = std::stoi(packet_data[0]);
            if(connection.message_id_map[messageID] == false){
                connection.message_id_map[messageID] = true;
            }         
            else{
                std::cout << "incorrect id";
                continue;
            }   
        }
        else if(std::holds_alternative<ErrorPacket>(recv_packet) || std::holds_alternative<NullPacket>(recv_packet)){
            //send bye packet and end
            ByePacket bye_packet;
            client->send(bye_packet.serialize());
            std::cout << "Error received, ending connection" << std::endl;
            exit(1);
           }
        else{
            //call the .getData() method of the packet and print the data
            std::cout << "Message received: \n";
            std::vector<std::string> packet_data = std::visit([&](auto& p) { return p.getData(); }, recv_packet);
            std::cout << packet_data[1] << std::endl;
            uint16_t messageID = std::stoi(packet_data[2]);
            ConfirmPacket confirm_packet(messageID);
            client->send(confirm_packet.serialize());
        }
        
        std::unique_lock<std::mutex> lock(reply_mutex);
        reply_cond_var.notify_one();
    }
}

void Runner::processAuthJoin(Connection &connection, std::string &reply, std::variant<RECV_PACKET_TYPE> recv_packet) {
    bool confirmed = false;
    bool replied = false;
    while(!confirmed || !replied) {
        if (std::holds_alternative<ReplyPacket>(recv_packet)) {
           ReplyPacket reply_packet = std::get<ReplyPacket>(recv_packet);
            std::vector<std::string> packet_data = reply_packet.getData();

            if (packet_data[0] != "1") {
                connection.clearAfterAuth();
                return; // No further processing needed if authentication failed
            }
            std::cout << "Message: " << packet_data[1] << std::endl;    
            ConfirmPacket confirm_packet(std::stoi(packet_data[2]));
            client->send(confirm_packet.serialize());
            replied = true;

        }
        else if (std::holds_alternative<ConfirmPacket>(recv_packet)) {
            ConfirmPacket confirm_packet = std::get<ConfirmPacket>(recv_packet);
            std::vector<std::string> packet_data = confirm_packet.getData();
            uint16_t messageID = std::stoi(packet_data[0]);
            if(connection.message_id_map[messageID] == false){
                connection.message_id_map[messageID] = true;
                confirmed = true;
            }         
            else{
                continue;
            } 
        }
        else if(std::holds_alternative<ErrorPacket>(recv_packet) || std::holds_alternative<NullPacket>(recv_packet)){
            //send bye packet and end
            ByePacket bye_packet;
            client->send(bye_packet.serialize());
            std::cout << "Error received, ending connection" << std::endl;
            exit(1);
        }

        if(confirmed && replied){
            break;
        }
        reply = client->receive();
        recv_packet = ReceiveParser(reply, connection);

    }
    std::cout << "\nAuthentication successful" << std::endl;
}


void Runner::run(){
    Connection connection = Connection(this->ip_address, this->port, this->protocol);

    std::cout << "Connected \n";
    
    // signal(SIGINT, handle_sigint);

     //thread for reading stdin, parsing them and sending them to other thread
     std::jthread inputThread([&](){
        inputScanner(connection);
    });

    //thread for sending messages to server
    std::jthread sendThread([&]() {
        //while first thread is running, this thread will be waiting for input
        if(connection.protocol == Connection::Protocol::UDP){
            packetSender(connection);
        }
        else if(connection.protocol == Connection::Protocol::TCP){
            packetSenderTCP(connection);
        }
    });
    
    std::jthread receiveThread([&]() {
        if(connection.protocol == Connection::Protocol::UDP){
            packetReceiver(connection);
        }
        else if(connection.protocol == Connection::Protocol::TCP){
            packetReceiverTCP(connection);
        }
    });
}