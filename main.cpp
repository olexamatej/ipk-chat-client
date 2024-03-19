#include "udp_client.h"
#include "inputParser.h"
#include "packet.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <vector>
#include <queue>
#include <condition_variable>


#include "runner.h"
//TODO

int main() {
    
    Runner("127.0.0.1", "4567", Connection::Protocol::UDP).run();


    return 0;
}