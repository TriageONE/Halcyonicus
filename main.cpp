#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <iostream>
#include <vector>

#define GLEW_STATIC
#define GLEW_NO_GLU
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
SDL_Window *window = 0;

void swap()
{
    SDL_GL_SwapWindow(window);
}

bool initOpenGL()
{

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cout << "Error initing SDL "<<SDL_GetError() << "\n";
        return true;
    }//It works now and the window doesnt ack-! itself

    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 ); //24 bits for fragment depth info, could be 16 but really shouldn't be, any other values probably won't work
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 ); //Use openGL version 4.0
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 0 );
    SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1); //Don't change this one
    SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, 4 ); //Controls anti-aliasing, 4x AA
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE); //Don't allow using deprecated functions

    //Last argument can be or'd with SDL_WINDOW_FULLSCREEN for full screen
    window = SDL_CreateWindow( "Triage's cool game!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                         SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if(!window)
    {
        std::cout<<"No window: "<<SDL_GetError()<<"\n";
        return true;
    }

    SDL_GLContext context = SDL_GL_CreateContext(window);

    if(!context)
    {
        std::cout<<SDL_GetError()<<" No context!\n";
        return true;
    }

    glewExperimental = GL_TRUE; //I dunno what this is, but I always use it lol


    GLenum glewError = glewInit();

    if(glewError != GLEW_OK)
    {
        std::cout<<"GlewInit failed "<<SDL_GetError()<<" , "<<glewError<<" , "<<glGetError()<<"\n";
    }

    SDL_GL_SetSwapInterval(true); //true for VSync

    return false;
}

//Takes two strings, you could load these from files if you want
GLuint createProgram(std::string vertexCode,std::string fragmentCode)
{
    //Allocate space for two shaders and tell OpenGL what kind they are
    GLuint vertShader,fragShader;
    vertShader = glCreateShader(GL_VERTEX_SHADER);
    fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    if(!vertShader)
    {
        std::cout<<"Couldn't allocate vertex shader!\n";
        return 0;
    }

    if(!fragShader)
    {
        std::cout<<"Couldn't allocate fragment shader!\n";
        return 0;
    }

    //Pass the strings to the graphics card and check for syntax errors
    /*glShaderSource(vertShader, 1,
                   reinterpret_cast<const GLchar *const *>(vertexCode.c_str()),
                   reinterpret_cast<const GLint *>(vertexCode.length()));
    glShaderSource(fragShader, 1,
                   reinterpret_cast<const GLchar *const *>(fragmentCode.c_str()),
                   reinterpret_cast<const GLint *>(fragmentCode.length()));*/

    char *vertBuffer = new char[vertexCode.length() + 1];
    strcpy(vertBuffer,vertexCode.c_str());
    vertBuffer[vertexCode.length()] = 0;
    int size = vertexCode.length();
    glShaderSource(vertShader,1,&vertBuffer,&size);
    delete vertBuffer;
    glCompileShader(vertShader);

    char *fragBuffer = new char[fragmentCode.length() + 1];
    strcpy(fragBuffer,fragmentCode.c_str());
    fragBuffer[fragmentCode.length()] = 0;
    size = fragmentCode.length();
    glShaderSource(fragShader,1,&fragBuffer,&size);
    delete fragBuffer;
    glCompileShader(fragShader);

    //Get the error log for the vertex shader
    int length,actualLength = 0;
    glGetShaderiv(vertShader,GL_INFO_LOG_LENGTH,&length);
    char *data = new char[length];
    glGetShaderInfoLog(vertShader,length,&actualLength,data);

    //If the length of the openGL error output is 0 then it succeeded
    if(!length)
        std::cout<<"Vert shader loaded okay!\n";
    else
    {
        std::cout<<"Error while compiling vertex see associated error file for more info!\n";
        std::cout<<data<<"\n";
    }

    delete data;

    //Get the error log for the fragment shader
    glGetShaderiv(fragShader,GL_INFO_LOG_LENGTH,&length);
    data = new char[length];
    glGetShaderInfoLog(fragShader,length,&actualLength,data);

    //If the length of the openGL error output is 0 then it succeeded
    if(!length)
        std::cout<<"Frag shader loaded okay!\n";
    else
    {
        std::cout<<"Error while compiling fragment see associated error file for more info!\n";
        std::cout<<data<<"\n";
    }

    delete data;

    GLuint program = glCreateProgram();
    if(program == 0)
    {
        std::cout<<"Couldn't allocate program!";
        return 0;
    }

    glAttachShader(program,vertShader);
    glAttachShader(program,fragShader);

    glLinkProgram(program);

    GLint programSuccess = GL_FALSE;
    glGetProgramiv( program, GL_LINK_STATUS, &programSuccess );
    if(programSuccess != GL_TRUE)
    {
        std::cout<<"Could not create/link program!\n";
        length = 0,actualLength = 0;
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &length );
        data = new char[length];
        glGetProgramInfoLog( program, length, &actualLength, data );

        if(length < 1)
            std::cout<<"Could not get error log for program compilation.\n";
        else
            std::cout<<data<<"\n";

        delete data;
        return 0;
    }

    return program;
}

//TODO: MAKE MORE THINGS HAPPEN
std::string vertShader = R"(
#version 400 core
layout(location = 0) in vec2 vertexPosition;
uniform vec3 drendran;
void main()
{
      gl_Position = vec4(vertexPosition,0.1,1);
}
)";

std::string fragShader = R"(
#version 400 core

uniform vec3 drendran;

out vec4 color;

void main()
{
    color = vec4(drendran,1);
}
)";

//TODO: MORE POINTS
GLuint createTriangleVAO()
{
    //A VAO holds buffers and 'metadata' so to speak describing the format and alignment of the buffers
    GLuint vao;
    glGenVertexArrays(1,&vao);

    //A buffer is a glorified array, ideally stored on the GPU's VRAM
    GLuint vertexBuffer;
    glGenBuffers(1,&vertexBuffer);

    //Say we're working on this particular VAO
    glBindVertexArray(vao);
    //Attach this particular buffer to this particular VAO, and say we're working on this given buffer
    glBindBuffer(GL_ARRAY_BUFFER,vertexBuffer);

    //What data we want to pass to our vertex shader
    std::vector<glm::vec2> points;
    points.push_back(glm::vec2(0,0));
    points.push_back(glm::vec2(0.5,1.0));
    points.push_back(glm::vec2(1.0,0));

    //Actually assign the data to the buffer, passing it to the GPU
    glBufferData(GL_ARRAY_BUFFER,sizeof(glm::vec2) * points.size(),&points[0],GL_STATIC_DRAW);

    //Say that VAO will use layout position 0, see the shader, layout = 0 is called vertexPosition
    glEnableVertexAttribArray(0);
    //Among other things, this says our buffer is made up of pairs (vec2s) of floats, and uses layout = 0
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,(void*)0);

    //Make sure future calls do not accidently affect our current VAO
    glBindVertexArray(0);

    std::cout<<glGetError()<<" error?\n";

    return vao;
}

int main(int argc, char* argv[])
{
    std::cout << "Starting..." << std::endl;
    bool quit = false;
    // Initialize SDL2 and create a window and renderer
    // :I maroonvent
    //KK, I think I know why let me see
    initOpenGL();

    // TODO: You may wish to create another function to encapsulate this one that loads the strings from files...
    GLuint program = createProgram(vertShader,fragShader);

    GLuint triangleVAO = createTriangleVAO();

    /*SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);*/
    unsigned long long frequency = SDL_GetPerformanceFrequency();
    unsigned long long startTime = SDL_GetPerformanceCounter();
    unsigned long long originTime = SDL_GetPerformanceCounter();
    // Load a texture for the box
    /*SDL_Surface* surface = IMG_Load("assets/deep_water.png");
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);*/

    // Define the box's position and size
    /*const int ogWidth = SCREEN_WIDTH / 2 - 50, ogHeight = SCREEN_HEIGHT / 2 - 50;

    SDL_Rect rect = { ogWidth, ogHeight, 100, 100 };

    // Define the rotation angle and rotation center
    float angle = 0.0f;
    int fps = 0;
    SDL_Point center = { 50, 50 };*/

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

        //Old triage code:
        /*int newWidth, newHeight;
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
        SDL_RenderPresent(renderer);*/

        //What color do we want to clear with RGBA
        glClearColor(0,0,1,0);
        //You can choose to clear only the color data, or the depth data, or both:
        //I.e. if you don't clear depth data, fragments will only render if they're in front of everything from last frame
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER,0);

        glUseProgram(program);
        glUniform3f(glGetUniformLocation(program,"drendran"),1,0.5,0);
        //Use our VAO with all its bound data
        glBindVertexArray(triangleVAO);
        //Tell OpenGL to draw it as triangles, and that there's 3 verts worth of data
        glDrawArrays(GL_TRIANGLES,0,3);
        glBindVertexArray(0);

        swap();
    }

    // Clean up resources
    /*SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);*/
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

