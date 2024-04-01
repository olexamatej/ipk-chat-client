#include "runner.h"
#define TIMEOUT 50
#include <signal.h>
#include <algorithm>

//global variable for sigint

Client *Runner::client = nullptr;
std::condition_variable queue_cond_var;

Runner::Runner(ArgumentParser argParser){
    this->ip_address = argParser.server_ip;
    this->port = std::to_string(argParser.port);
    this->protocol = argParser.protocol;
    this->timeout = argParser.timeout;
    this->retries = argParser.retries;

    if(this->protocol == Connection::Protocol::UDP){
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




void Runner::inputScanner(Connection &connection){
    std::string line;
    //Scans for input, parses it and adds it to Q
    if(connection.protocol == Connection::Protocol::UDP){
        while (std::getline(std::cin, line)) {
            Input userInput;
            userInput.getNewInput(line);
            
            if(connection.exit_flag != -1){
                return;
            }
            
            std::lock_guard<std::mutex> lock(queue_mutex);
        
            input_packet_queue.push(userInput.parseInput(connection));
            //if userInput is nullpacket, then continue
            if(std::holds_alternative<NullPacket>(input_packet_queue.back())){
                //check if rename in NullPacket is true
                if(!std::get<NullPacket>(input_packet_queue.back()).rename){
                    std::cerr << "ERR: Invalid input" << std::endl;
                }
                input_packet_queue.pop();
                continue;
            }    
            //sets message_id_map to false, waits for confirm in different thread
            connection.message_id_map[connection.message_id] = false;
            //notifying send thread, so it can send parsed message
            queue_cond_var.notify_one(); 
        }
        std::cerr << "ERR: End of input\n";
        connection.exit_flag = 0;
        endUDPConnection(connection);

        return;
    }
    else if(connection.protocol == Connection::Protocol::TCP){
         while (std::getline(std::cin, line)) {
            Input userInput;
            userInput.getNewInput(line);
            if(connection.exit_flag != -1){
                return;
            }
            std::lock_guard<std::mutex> lock(queue_mutex);
        
            input_packet_queue.push(userInput.parseInput(connection));
            //if userInput is nullpacket, then continue
            if(std::holds_alternative<NullPacket>(input_packet_queue.back())){
                if(!std::get<NullPacket>(input_packet_queue.back()).rename){
                    std::cerr << "ERR: Invalid input" << std::endl;
                }
                input_packet_queue.pop();
                continue;
            }    
            //notifying send thread, so it can send parsed message

            queue_cond_var.notify_one(); 
        }
        
        std::cerr << "ERR: End of input\n";
    
        connection.exit_flag = 0;
        
        return;
    }
}

//Packet sender for TCP variant
void Runner::packetSenderTCP(Connection &connection){
           while(1) {
            if(connection.exit_flag != -1){
                return;
            }
            std::unique_lock<std::mutex> lock(queue_mutex);
            if (!queue_cond_var.wait_for(lock, std::chrono::milliseconds(100), [&] { return !input_packet_queue.empty() || connection.exit_flag != -1; })) {
                continue; // Continue to next iteration of the loop
            }

              if (connection.exit_flag != -1 || input_packet_queue.empty()) {
                return;
            }

            //get the first packet from the queue and call .serialize
            send_packet = input_packet_queue.front();

            std::string serialized_packet = "";
            std::visit([&](auto& p) { serialized_packet = p.serialize(connection); }, send_packet);
            //serialize it
            input_packet_queue.pop();
            lock.unlock();
            //send packet

            client->send(serialized_packet);

            if(std::holds_alternative<AuthPacket>(send_packet) || std::holds_alternative<JoinPacket>(send_packet)){

                std::unique_lock<std::mutex> lock(reply_mutex);
                reply_cond_var.wait(lock);
            }
        }
}



//receiver for TCP
void Runner::packetReceiverTCP(Connection &connection){

         while(1) {
                if(connection.exit_flag != -1){
                return;
            }
            std::string reply = client->receive();

            if(reply ==""){
                continue;
            }
            std::variant<RECV_PACKET_TYPE> recv_packet = ReceiveParser(reply, connection);
            std::vector<std::string> packet_data;
            std::visit([&](auto& p) { packet_data = p.getData(); }, recv_packet);
            if(std::holds_alternative<ReplyPacket>(recv_packet) ){
                if(!(std::holds_alternative<AuthPacket>(send_packet) || std::holds_alternative<JoinPacket>(send_packet))){
                    std::cerr  << "ERR: Invalid packet received, ending connection" << std::endl;
                    exit(1);
                }                
                //checks if it was success
                if(packet_data[0] == "1"){
                    std::cerr << "Success: ";
                } 
                else {
                    if(std::holds_alternative<AuthPacket>(send_packet)){
                        connection.clearAfterAuth(); 
                    }    
                    std::cerr << "Failure: ";
                }

                std::cerr << packet_data[1] << std::endl;
            }
            // 
            else if(std::holds_alternative<ErrorPacket>(recv_packet)){
                //send bye packet and end
                ByePacket bye_packet;
                client->send(bye_packet.serialize(connection));
                std::cerr << "ERR FROM " << packet_data[0] << ": " << packet_data[1] << std::endl;
                connection.exit_flag = 1;
            }
            //this is packet for invalid messages
            else if(std::holds_alternative<NullPacket>(recv_packet)){
                
                ErrorPacket error_packet("Invalid packet received", connection.display_name);
                client->send(error_packet.serialize(connection));

                std::cerr << "ERR: Invalid packet received" << std::endl;
                ByePacket bye_packet;
                client->send(bye_packet.serialize(connection));
                connection.exit_flag = 1;
            }
            else if(std::holds_alternative<MsgPacket>(recv_packet)){
                    
                std::cout << packet_data[0] << ": "<< packet_data[1] << std::endl;
            }
            else{
                std::cerr << "ERR: invalid message\r\n";
                connection.exit_flag = 1;
            }
            
            std::unique_lock<std::mutex> lock(reply_mutex);
            reply_cond_var.notify_one();
        }
}

void Runner::endUDPConnection(Connection &connection){
    ByePacket byePacket;

    client->send(byePacket.serialize(connection));
    //
    
    client->receiveTimeout(connection.timeout);

    for(int i = 0; i < this->retries; i++){
        client->send(byePacket.serialize(connection));

        //if reply received is confirm, check id
        std::string reply = client->receive();
        if(reply == ""){
            continue;
        }
        std::variant<RECV_PACKET_TYPE> recv_packet = ReceiveParser(reply, connection);


        //if recv_packet is confirm packet, then check id
        if(std::holds_alternative<ConfirmPacket>(recv_packet)){
            ConfirmPacket confirm_packet = std::get<ConfirmPacket>(recv_packet);
            std::vector<std::string> packet_data = confirm_packet.getData();
            uint16_t messageID = std::stoi(packet_data[0]);
            //if messageID is not in message_id_map, then continue
            if(connection.message_id_map[messageID] == false){
                connection.message_id_map[messageID] = true;
            }
            //if every id is confirmed, then break
            if(std::all_of(connection.message_id_map.begin(), connection.message_id_map.end(), [](auto &p) { return p.second == true; })){
                break;
            }
        }
        else if(std::holds_alternative<ErrorPacket>(recv_packet)){
            std::vector<std::string> packet_data = std::visit([&](auto& p) { return p.getData(); }, recv_packet);
            std::cerr << "ERR FROM " << packet_data[0] << ": " << packet_data[1] << std::endl;
            connection.exit_flag = 1;
            break;
        }
    }
    connection.exit_flag = 0;
}


//packet sender for udp
void Runner::packetSender(Connection &connection) {
        while(1) {
            if(connection.exit_flag != -1){
                return;
            }
            std::unique_lock<std::mutex> lock(queue_mutex);

            // Blocking wait with timeout
            if (!queue_cond_var.wait_for(lock, std::chrono::milliseconds(100), [&] { return !input_packet_queue.empty() || connection.exit_flag != -1; })) {
                continue; // Continue to next iteration of the loop
            }

            // Check exit flag after waiting
            if (connection.exit_flag != -1 || input_packet_queue.empty()) {
                return;
            }
            

            //get the first packet from the queue and call .serialize
            send_packet = input_packet_queue.front();

            std::string serialized_packet = "";
            std::visit([&](auto& p) { serialized_packet = p.serialize(connection); }, send_packet);

            input_packet_queue.pop();
            lock.unlock();
            //attempts
        for (int attempt = 0; attempt < this->retries+1; ++attempt) {
                client->send(serialized_packet);
                std::unique_lock<std::mutex> reply_lock(reply_mutex);
                if(reply_cond_var.wait_for(reply_lock, std::chrono::milliseconds(this->timeout)) == std::cv_status::timeout) {
                } else {
                   if(replied.load()) {
                         break; // Break out of the loop if the message was sent successfully and all messages have been confirmed
                    }
                }
            }
        }
        // If the loop finished without breaking, it means that the message wasn't sent successfully
        if (!replied.load()) {
            std::cerr << "Failed to send message after " << this->retries << " attempts. Exiting...\n";
            exit(1);
        }

    }

//UDP message receiver
void Runner::packetReceiver(Connection &connection) {
    while (1) {
        if(connection.exit_flag != -1){
            return;
        }
        std::string reply = client->receive();
        if(reply == ""){
            continue;
        }
        std::variant<RECV_PACKET_TYPE> recv_packet = ReceiveParser(reply, connection);

        if (std::holds_alternative<AuthPacket>(send_packet) || std::holds_alternative<JoinPacket>(send_packet)) {
            //after sending auth/join it will not exit this loop until it is either confirmed or timeout has passed
            replied.store(false);
            // processAuthJoin(connection, reply, recv_packet);
            if (std::holds_alternative<ReplyPacket>(recv_packet)) {
                
                ReplyPacket reply_packet = std::get<ReplyPacket>(recv_packet);
                std::vector<std::string> packet_data = reply_packet.getData();
   
                if (packet_data[0] == "0") {
                    std::cerr << "Failure: " << packet_data[1] << "\n";
                    connection.clearAfterAuth();
                }
                else{
                    std::cerr << "Success: " << packet_data[1] << "\n";
                }
                replied.store(true);

                ConfirmPacket confirm_packet(std::stoi(packet_data[2]));
                client->send(confirm_packet.serialize(connection));
                send_packet = NullPacket();
            }
        }
        if(std::holds_alternative<ConfirmPacket>(recv_packet)){
            ConfirmPacket confirm_packet = std::get<ConfirmPacket>(recv_packet);
            std::vector<std::string> packet_data = confirm_packet.getData();
            uint16_t messageID = std::stoi(packet_data[0]);
            //if messageID is not in message_id_map, then continue

            if(connection.message_id_map[messageID] == false){
                connection.message_id_map[messageID] = true;
            }         
        }
        else if(std::holds_alternative<ErrorPacket>(recv_packet)){
            
            //send confirm packet with same ID as recv_packet
            std::vector<std::string> packet_data = std::visit([&](auto& p) { return p.getData(); }, recv_packet);

            ConfirmPacket confirm_packet(std::stoi(packet_data[2]));
            client->send(confirm_packet.serialize(connection));

            ByePacket bye_packet;
            client->send(bye_packet.serialize(connection));
            //TODO fix
            std::cerr << "ERR FROM " << packet_data[0] << ": " << packet_data[1] << std::endl;
            connection.exit_flag = 1;

        }
        //imaginary packet - invalid
        else if(std::holds_alternative<NullPacket>(recv_packet)){

            std::vector<uint8_t> data_bytes(reply.begin(), reply.end());\
            if(data_bytes.size() < 2){
                continue;
            }
            uint8_t first_byte = data_bytes[1];
            uint8_t second_byte = data_bytes[2];
            //put the bytes in a uint16_t

            uint16_t messageID = (first_byte << 8) | second_byte;

            ConfirmPacket confirm_packet(messageID);
            client->send(confirm_packet.serialize(connection));

            ErrorPacket error_packet("Invalid packet received", connection.display_name);
            client->send(error_packet.serialize(connection));
            std::cerr << "ERR: Invalid packet received" << std::endl;
            endUDPConnection(connection);

            connection.exit_flag = 1;
           }
        else if (std::holds_alternative<MsgPacket>(recv_packet)){
            std::vector<std::string> packet_data = std::visit([&](auto& p) { return p.getData(); }, recv_packet);
            
            uint16_t messageID = std::stoi(packet_data[2]);
            
            //if messageID is in message_id_map, then continue
            if(connection.message_id_map[messageID] == true){
                continue;
            }
            std::cout << packet_data[0] << ": " << packet_data[1] << std::endl;
            ConfirmPacket confirm_packet(messageID);
            client->send(confirm_packet.serialize(connection));
        }
        
        //if there isnt any message that hasnt been confirmed, then send the next message

        if(std::all_of(connection.message_id_map.begin(), connection.message_id_map.end(), [](auto &p) { return p.second == true; }) &&
            replied == true){
            std::unique_lock<std::mutex> lock(reply_mutex);
            reply_cond_var.notify_one();
        }
   
    }
}

Connection connection;
std::mutex exit_flag_mutex;


//handling sigint
void handle_sigint(int sig) {

    // delete(Runner::client);
    exit(0);
}

int Runner::run(){

    connection.ip_address = this->ip_address;
    connection.port = this->port;
    connection.protocol = this->protocol;
    connection.timeout = this->timeout;
    connection.retries = this->retries;

    // std::cout << "Connected \n";
    
    signal(SIGINT, handle_sigint);

   
     //thread for reading stdin, parsing them and sending them to other thread
     std::thread inputThread([&](){
        //get self pthread id
        inputScanner(connection);
    });

    //thread for sending messages to server
    std::thread sendThread([&]() {
        //get self pthread id

        //while first thread is running, this thread will be waiting for input
        if(connection.protocol == Connection::Protocol::UDP){
            packetSender(connection);
        }
        else if(connection.protocol == Connection::Protocol::TCP){
            packetSenderTCP(connection);
        }
    });
    //thread for receiving messages from server
    std::thread receiveThread([&]() {
        if(connection.protocol == Connection::Protocol::UDP){
            packetReceiver(connection);
        }
        else if(connection.protocol == Connection::Protocol::TCP){
            packetReceiverTCP(connection);
        }
    });

    std::mutex exit_flag_mutex;  // Define the mutex

    auto monitorExitFlag = [&]() {
        while (true) {
            // Sleep for a short duration to prevent busy waiting

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            // Check exit_flag in a thread-safe manner
            int exit_flag;
            {
                std::lock_guard<std::mutex> lock(exit_flag_mutex);
                exit_flag = connection.exit_flag;
            }

            // If exit_flag is not -1, notify the condition variable
            if (exit_flag != -1) {
                queue_cond_var.notify_all();
                return;
            }
        }
    };

    
    inputThread.join();
    sendThread.join();
    receiveThread.join();

    return connection.exit_flag;

}