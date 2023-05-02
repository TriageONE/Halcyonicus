#include <random>
#include <iostream>
#include <string>
#include <sstream>


#include "lib/net/halNet.h"
#include "lib/tools/streamtools.h"
#include "lib/net/halNetP.h"
#include "lib/crypto/base64.h"
#include <thread>
#include <mutex>
#include <unistd.h>

using namespace std;

static int randomPositiveNumber() {
    std::random_device rd; // create a random device to seed the generator
    std::mt19937 gen(rd()); // create a Mersenne Twister generator seeded with the random device
    std::uniform_int_distribution<> dist(0, 99999999); // create a uniform distribution between 0 and 99,999,999
    int randomNumber = dist(gen); // generate a random number using the generator and distribution
    return randomNumber;
}

bool stop = false; // shared variable to signal the worker thread to stop
std::mutex stopMutex;
std::mutex listenMutex;

int main() {

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    //Does not work for no good reason
    //_setmode(_fileno(stdout), _O_U16TEXT);
    // Enable buffering to prevent VS from chopping up UTF-8 byte sequences
    setvbuf(stdout, nullptr, _IOFBF, 2000);
#endif

    for (int i = 0; i <= 20; i++){
        cout << randomPositiveNumber() << endl;
    }

    HALNET server;
    // create a new thread and start running the helloWorldThread function
    std::thread t( [&server] () {
        while (!stop){
            cout << "SRV: listening.." << endl;
            server.listen();
        }
        cout << "SRV: Server Exited" << endl;
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(500));


    cout << "CLI: Starting client.. " << endl;
    HALNET_P client("127.0.0.2");
    client.connect();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));


    std::thread c( [&client] () {
        cout << "CLI: Client Listener started.." << endl;
        while (!stop){
            cout << "CLI: listening.." << endl;

            client.listen();
        }
        cout << "CLI: Client Exited" << endl;
    });

    sleep(1);
    cout << "CLI: Connected client, now attempting assoc.." << endl;
    client.associate();

    // main thread continues running while the other thread is running in the background
    int i = 0;
    sleep(2);
    while (i < 500) {
        client.sendTest();
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        i++;
    }
    stopMutex.lock();

    stop = true;
    client.disconnect();
    stopMutex.unlock();

    t.join();
    c.join();
    // wait for the other thread to finish before exiting the program */
    return 0;

}
