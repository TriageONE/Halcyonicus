//
// Created by Triage on 5/7/2023.
//

#ifndef HALCYONICUS_HALUI_H
#define HALCYONICUS_HALUI_H


#include <chrono>
#include <thread>
#include <cmath>

class HALUI{

    struct DIMENSION{
        int x;
        int y;
    } d{};
    int current_refresh_rate = 30;


    bool stop_queue = false;
    bool pause_queue = false;

    double rollingFPSAverage[8]{0.0};
    char rollingTracker = 0;
    double lastTime;


public:

    HALUI(int width, int height){


        //Display data
        d.x= width; d.y = height;

        std::cout << "HAL_UI_INFO: Finished setup!" << std::endl;


    }

    ~HALUI() {

        std::cout << "HAL_UI_INFO: Finished deconstruction!" << std::endl;

    }

    void process(){

    }


    [[maybe_unused]] void stop(){
        this->stop_queue = true;
    }

    [[maybe_unused]] void start(){
        this->stop_queue = false;
    }

private:

    void wait(int milliseconds) {
        std::chrono::milliseconds duration(milliseconds);
        std::this_thread::sleep_for(duration);
    }

    double fps(double lastFramePoint){
        auto time = //TODO: replace with delta time
        return 1 / (time - lastFramePoint);
    }

    std::string averageFPS(){
        double accm = 0;
        for (double av : rollingFPSAverage){
            accm += av;
        }
        return std::to_string( ceil(accm / 32) );
    }

};
#endif //HALCYONICUS_HALUI_H
