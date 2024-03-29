#ifndef RUNNER_H
#define RUNNER_H
#include "udp_client.h"
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
#include "argumentParser.h"
#include <variant>

class Runner {
    public:
        Runner(ArgumentParser argParser);
        ~Runner();
        void run();
        void inputScanner(Connection &connection);
        void packetSender(Connection &connection);
        void packetSenderTCP(Connection &connection);
        void packetReceiver(Connection &connection);
        void packetReceiverTCP(Connection &connection);

        void processAuthJoin(Connection &connection, std::string &reply, std::variant<RECV_PACKET_TYPE> recv_packet);
        void handleReplyPacket(Connection &connection, std::string &reply, std::variant<RECV_PACKET_TYPE> recv_packet);
        void handleConfirmPacket(Connection &connection, std::string &reply, std::variant<RECV_PACKET_TYPE> recv_packet);


        std::mutex queue_mutex;
        std::condition_variable queue_cond_var;
        std::condition_variable reply_cond_var;
        std::mutex reply_mutex;
        std::variant<PACKET_TYPE> send_packet = NullPacket();
        std::queue<std::variant<PACKET_TYPE>> input_packet_queue;

        static Client *client;
     
    private:

        std::string ip_address;
        std::string port;
        uint16_t timeout;
        uint8_t retries;
        Connection::Protocol protocol;
};


#endif