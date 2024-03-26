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

#include "argumentParser.h"
#include "runner.h"

int main(int argc, char *argv[]) {
    ArgumentParser argParser(argc, argv);

    Runner(argParser).run();


    return 0;
}