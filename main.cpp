#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <thread>

#include "graphics/initOpengl.h"
#include "graphics/camera.h"
#include "graphics/texture.h"
#include "lib/world/chunk.h"
#include "lib/world/generator.h"

int SCREEN_WIDTH = 1920;
int SCREEN_HEIGHT = 1080;

int main(int argc, char* argv[])
{
    std::cout << "Starting..." << std::endl;
    bool quit = false;
    // Initialize SDL2 and create a window and renderer
    SDL_Window *window = initOpenGL(SCREEN_WIDTH,SCREEN_HEIGHT);

    GLuint program = loadDefaultShaders();

    std::vector<chunk*> chunks;
    puts("Attempting chunk creation");
    chunk myChunk (program, 0, 0);
    puts("Chunk creation succeeded");
    chunks.push_back(&myChunk);

    //std::thread progressiveBuilder( [&chunks, &program] () {
        //std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        GENERATOR::applyPhase1({}, &myChunk);
        chunk chunkTwo(program,0,3);
        chunks.push_back(&chunkTwo);
        auto data = chunks[0]->serialize2();
        std::cout<<"Chunk1 serial size: "<< data.size() <<"\n";
        chunkTwo.deserialize2(data);
        //return;
    //});

    //Report all locations of current chunk data in chunk 1
    int tracker = 0;
    //for (auto c : chunks.at(0)->cubeMap){
        //std::cout << "Block: " << tracker << "\n\tTYPE: " << c.second.type << "\n\tX: " << (int)c.second.offsetX << "\n\tY: " << (int)c.second.offsetY << "\n\tZ: " << (int)c.second.offsetZ << std::endl;
        //tracker++;
   // }/

    unsigned long long frequency = SDL_GetPerformanceFrequency();
    unsigned long long startTime = SDL_GetPerformanceCounter();
    unsigned long long originTime = SDL_GetPerformanceCounter();
    int screenWidth = SCREEN_WIDTH, screenHeight = SCREEN_HEIGHT;

    bool mouseLocked = true;

    camera me(program);
    me.setAspectRatio(SCREEN_WIDTH,SCREEN_HEIGHT);
    SDL_SetRelativeMouseMode(mouseLocked ? SDL_TRUE : SDL_FALSE);

    GLint atlasRows = glGetUniformLocation(program,"atlasRows");
    GLint atlasColumns = glGetUniformLocation(program,"atlasColumns");

    atlas test(2,2,32);
    test.addFile("assets/grass.png");
    test.addFile("assets/stone.png");
    test.addFile("assets/water.png");
    test.compile();

    glEnable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);

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
                    if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                    {
                        SDL_GetWindowSize(window, &screenWidth, &screenHeight);
                        std::cout << "NEW SIZE: " << screenWidth << "x" << screenHeight << std::endl;
                        me.setAspectRatio(screenWidth,screenHeight);
                    }
                    break;
                case SDL_MOUSEMOTION:
                    if(mouseLocked)
                        me.takeMouseInput(event.motion.xrel * 2 ,event.motion.yrel * 2);
                    break;
                case SDL_KEYDOWN:
                    if(event.key.keysym.sym == SDLK_m)
                    {
                        mouseLocked = !mouseLocked;
                        SDL_SetRelativeMouseMode(mouseLocked ? SDL_TRUE : SDL_FALSE);
                    }
                    break;
            }
        }

        me.moveAround(delta);

        //What color do we want to clear with RGBA
        glClearColor(0,0,1,0);
        //You can choose to clear only the color data, or the depth data, or both:
        //I.e. if you don't clear depth data, fragments will only render if they're in front of everything from last frame
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0,0,screenWidth,screenHeight);
        glBindFramebuffer(GL_FRAMEBUFFER,0);

        glUseProgram(program);
        me.render();
        GLint timePassed = glGetUniformLocation(program,"time");
        glUniform1f(timePassed, ((float)SDL_GetTicks()) * 0.01);

        glUniform1i(atlasRows,test.atlasRows);
        glUniform1i(atlasColumns,test.atlasColumns);
        glUniform1i(glGetUniformLocation(program,"myAtlas"),0);
        glBindTexture(GL_TEXTURE_2D,test.handle);

        for(int a = 0; a<chunks.size(); a++)
            chunks[a]->render();

        SDL_GL_SwapWindow(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    //progressiveBuilder.join();
    return 0;
}
