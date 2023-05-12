//
// Created by Triage on 5/8/2023.
//

#ifndef HALCYONICUS_WINDOW_H
#define HALCYONICUS_WINDOW_H

#include "panel.h"
#include "../halui.h"
#include <iostream>
#include <algorithm>

class HAL_WINDOW{

    //Windows have a length but not a height, because the top is constant height
    float length;

    //Windows contain exactly one panel that can be rendered below the window header
    HAL_PANEL childPanel;

    //Windows have a location they occupy that is initialized on startup
    float x{}, y{};

    //Each window should be found by its UUID
    long long uuid;

    //Each window should always be able to
    bool grabPoint, closeMarker, valid;

    //Each window should also have a color


public:

    /**
     * Defines a window to have one panel as a component, or nothing, as a blank window.
     * Initializes the following automatically:
     * length = 128
     * grabPoint = true
     * closeMarker = true
     *
     */
    HAL_WINDOW(float locationX, float locationY, /*TODO: color,*/ long long uuid){
        this->x = locationX;
        this->y = locationY;
        this->uuid = uuid;
        //this->color = color;

        length = 128;
        grabPoint = true;
        closeMarker = true;
    }

    HAL_WINDOW(float locationX, float locationY, /*TODO: color,*/ long long uuid, float length){
        this->x = locationX;
        this->y = locationY;
        this->uuid = uuid;
        //this->color = color;

        grabPoint = true;
        closeMarker = true;
        setLength(length);
    }

    HAL_WINDOW(float locationX, float locationY, /*TODO: color,*/ long long uuid, float length, bool grabPoint, bool closeMarker){
        this->x = locationX;
        this->y = locationY;
        this->uuid = uuid;
        //this->color = color;

        this->grabPoint = grabPoint;
        this->closeMarker = closeMarker;
        setLength(length);
    }

    ~HAL_WINDOW() = default;

    void setPanel(HAL_PANEL panel){
        this->childPanel = panel;
    }

    HAL_PANEL * getPanel(){
        return &this->childPanel;
    }

    [[nodiscard]] bool hasGrabPoint() const{
        return grabPoint;
    }

    [[nodiscard]] bool hasCloseMarker() const{
        return closeMarker;
    }

    void setLocation(int newX, int newY){
        this->x = newX;
        this->y = newY;
    }

    void setLength(int newLen){
        int minLen = 1;
        if (grabPoint) minLen += 24;
        if (closeMarker) minLen += 16;
        std::clamp(newLen, minLen, 8192);
        this->length = newLen;
    }

    void render(HALUI * ui){
        //Should render the panel. Expensive. Only use if you know there needs to be an update
        //Rendering validates the panel after displaying it

        //TODO:
        // - Render the window header

        //If it doesnt have a grabpoint and close marker, pass. Render the rest and not the header
        if (closeMarker || grabPoint){
            al_draw_filled_rectangle(x, y, x + length, y + 16, color);
            if (closeMarker){
                aldra
            }
        }
        // - Broadcast invalidation to all below

        // We should broadcast this invalidation to everyone below, since how else are we going to render the buttons
        makeValid();
    }

    void invalidate(){
        valid = false;
    }

    void makeValid(){
        valid = true;
    }

    bool isValid(){
        return valid;
    }

};
#endif //HALCYONICUS_WINDOW_H
