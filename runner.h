#include "udp_client.h"
#include "inputParser.h"
#include "packet_tcp.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <vector>
#include <signal.h>
#include <queue>
#include <condition_variable>

class Runner {
    public:
        Runner(std::string ip_address, std::string port, Connection::Protocol protocol)
        : ip_address(ip_address), port(port), protocol(protocol), client(ip_address, port) {
        }
        void run();
        void handle_sigint(int sig);
        std::mutex queue_mutex;
        std::condition_variable queue_cond_var;
        std::condition_variable reply_cond_var;
        std::mutex reply_mutex;
        std::variant<PACKET_TYPE> send_packet = NullPacket();
        std::queue<std::variant<PACKET_TYPE>> input_packet_queue;
        
    private:
        std::string ip_address;
        std::string port;
        Connection::Protocol protocol;
        UDPClient client;
};