#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <iostream>
#include <thread>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


int main(int argc, char* argv[])
{
    bool quit = false;
    // Initialize SDL2 and create a window and renderer

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    unsigned long long frequency = SDL_GetPerformanceFrequency();
    unsigned long long startTime = SDL_GetPerformanceCounter();
    unsigned long long originTime = SDL_GetPerformanceCounter();
    // Load a texture for the box
    SDL_Surface* surface = IMG_Load("assets/deep_water.png");
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    // Define the box's position and size
    const int ogWidth = SCREEN_WIDTH / 2 - 50, ogHeight = SCREEN_HEIGHT / 2 - 50;

    SDL_Rect rect = { ogWidth, ogHeight, 100, 100 };

    // Define the rotation angle and rotation center
    float angle = 0.0f;
    int fps = 0;
    SDL_Point center = { 50, 50 };

    // Enter the main loop

    while (!quit)
    {
        Uint64 currentTime = SDL_GetPerformanceCounter();
        double delta = ((double)(currentTime - startTime) / frequency) * 100;
        double sinceStart = ((double)(currentTime - originTime) / frequency);
        startTime = currentTime;
        // Handle events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_MOVED)
                    {
                        // The window has been moved
                        // Ignore this event and continue rendering
                    }

            }
        }

        // Update the angle and wrap it around at 360 degrees
        angle += 1.0f * delta ;
        if (angle >= 360.0f)
        {
            angle -= 360.0f;
        }

        int newWidth, newHeight;
        newWidth = ogWidth + (sin(sinceStart*20)*10);
        newHeight = ogHeight + (sin(sinceStart*20)*-10);

        rect.w = newWidth;
        rect.h = newHeight;

        rect.x = newHeight / 2;
        rect.y = newWidth / 2;

        // Clear the renderer with black
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render the rotated box
        SDL_RenderCopyEx(renderer, texture, NULL, &rect, angle, &center, SDL_FLIP_NONE);
        // Present the renderer to the window
        SDL_RenderPresent(renderer);
    }

    // Clean up resources
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
