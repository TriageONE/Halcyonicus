#include <iostream>
#include <thread>


void listener()
{

}

int main()
{
    std::thread listener_thread(listener);
    listener_thread.detach();


    return 0;
}
