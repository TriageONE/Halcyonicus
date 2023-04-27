#include <iostream>
#include <string>
#include <sstream>

#include "lib/net/halNet.h"
#include "lib/net/halNetP.h"
#include <thread>
#include <chrono>
#include <mutex>

using namespace std;

bool stop = false; // shared variable to signal the worker thread to stop
std::mutex stopMutex;

int main() {

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    //Does not work for no good reason
    //_setmode(_fileno(stdout), _O_U16TEXT);
    // Enable buffering to prevent VS from chopping up UTF-8 byte sequences
    setvbuf(stdout, nullptr, _IOFBF, 2000);
#endif

    HALNET server;
    // create a new thread and start running the helloWorldThread function
    std::thread t( [&server] () {
        while (!stop){
            server.listen();
        }

        cout << "SRV: Server Exited" << endl;
    });

    cout << "CLI: Starting client.. " << endl;
    HALNET_P client("127.0.0.1");
    cout << "CLI: Started client" << endl;

    client.connect();
    cout << "CLI: Connected client" << endl;

    // main thread continues running while the other thread is running in the background
    int i = 0;
    sleep(2);
    while (i < 5) {
        stringstream msg;
        msg << "Sent Packet " << i ;
        client.send(msg.str());
        cout << "CLI: Sent packet containing: \"" << msg.str() << endl;
        i++;
        sleep(1);
    }
    stopMutex.lock();

    stop = true;
    client.disconnect();
    stopMutex.unlock();
    // wait for the other thread to finish before exiting the program
    t.join();

    return 0;

}
