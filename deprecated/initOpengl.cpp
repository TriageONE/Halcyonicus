//
// Created by Triage on 5/25/2023.
//

#include "../halcyonicus.h"

SDL_Window *initOpenGL(int resX,int resY)
{
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cout << "Error initing SDL "<<SDL_GetError() << "\n";
        return 0;
    }

    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 ); //24 bits for fragment depth info, could be 16 but really shouldn't be, any other values probably won't work
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 ); //Use openGL version 4.0
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 0 );
    SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1); //Don't change this one
    SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, 4 ); //Controls anti-aliasing, 4x AA
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE); //Don't allow using deprecated functions

    //Last argument can be or'd with SDL_WINDOW_FULLSCREEN for full screen
    SDL_Window *window = SDL_CreateWindow( "Halcyonicus", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                               resX, resY, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if(!window)
    {
        std::cout<<"No window: "<<SDL_GetError()<<"\n";
        return 0;
    }

    SDL_GLContext context = SDL_GL_CreateContext(window);

    if(!context)
    {
        std::cout<<SDL_GetError()<<" No context!\n";
        return 0;
    }

    glewExperimental = GL_TRUE; //I dunno what this is, but I always use it lol

    GLenum glewError = glewInit();

    if(glewError != GLEW_OK)
    {
        std::cout<<"GlewInit failed "<<SDL_GetError()<<" , "<<glewError<<" , "<<glGetError()<<"\n";
    }

    SDL_GL_SetSwapInterval(true); //true for VSync

    return window;
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

    //Pass the code we wrote to the graphics card for the first shader
    char *vertBuffer = new char[vertexCode.length() + 1];
    strcpy(vertBuffer,vertexCode.c_str());
    vertBuffer[vertexCode.length()] = 0;
    int size = vertexCode.length();
    glShaderSource(vertShader,1,&vertBuffer,&size);
    delete[] vertBuffer;
    glCompileShader(vertShader);

    //Pass the code we wrote to the graphics card for the second shader
    char *fragBuffer = new char[fragmentCode.length() + 1];
    strcpy(fragBuffer,fragmentCode.c_str());
    fragBuffer[fragmentCode.length()] = 0;
    size = fragmentCode.length();
    glShaderSource(fragShader,1,&fragBuffer,&size);
    delete[] fragBuffer;
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

    delete[] data;

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

GLuint loadDefaultShaders(std::string vertexShaderPath,std::string fragmentShaderPath)
{
    std::ifstream vertShader(vertexShaderPath.c_str(),std::ios::ate | std::ios::binary);
    if(!vertShader.is_open())
    {
        std::cout<<"Error, could not open vertex shader!\n";
        return 0;
    }

    //Figure out how big file is then return to start position
    int size = vertShader.tellg();
    vertShader.seekg(0);

    std::cout<<"Vertex shader size: "<<size<<"\n";

    //Load strings from file
    char *data = new char[size+1];
    vertShader.read(data,size);
    data[size] = 0; //Buffer terminated for sure
    std::string vertBuf = std::string(data);
    delete data;

    vertShader.close();

    //Frag shader
    std::ifstream fragShader(fragmentShaderPath.c_str(),std::ios::ate | std::ios::binary);
    if(!fragShader.is_open())
    {
        std::cout<<"Error, could not open fragment shader!\n";
        return 0;
    }

    //Figure out how big file is then return to start position
    size = fragShader.tellg();
    fragShader.seekg(0);

    std::cout<<"Fragment shader size: "<<size<<"\n";

    data = new char[size+1];
    fragShader.read(data,size);
    data[size] = 0; //Buffer terminated for sure
    std::string fragBuf = std::string(data);
    delete data;

    fragShader.close();

    return createProgram(vertBuf,fragBuf);
}
