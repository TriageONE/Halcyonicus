#include <iostream>
#include <string>
#include <cstdio>

#include "lib/net/halNet.h"
#include "lib/net/halNetP.h"
#include <thread>
#include <chrono>

using namespace std;

[[noreturn]] void helloWorldThread() {
    while (true) {
        std::cout << "Hello, world!" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}


int main() {

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    //Does not work for no good reason
    //_setmode(_fileno(stdout), _O_U16TEXT);
    // Enable buffering to prevent VS from chopping up UTF-8 byte sequences
    setvbuf(stdout, nullptr, _IOFBF, 2000);
#endif

    // create a new thread and start running the helloWorldThread function
    std::thread t(helloWorldThread);

    // main thread continues running while the other thread is running in the background
    while (true) {
        // do some other work in the main thread
        // ...
    }

    // wait for the other thread to finish before exiting the program
    t.join();

    return 0;

}
