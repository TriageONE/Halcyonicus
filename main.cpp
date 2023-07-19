#define SDL_MAIN_HANDLED
#include "halcyonicus.h"

#include "graphics/initOpengl.h"
#include "graphics/camera.h"

int SCREEN_WIDTH = 1920;
int SCREEN_HEIGHT = 1080;

int main(int argc, char* argv[])
{
    std::cout << "Starting..." << std::endl;
    bool quit = false;
    // Initialize SDL2 and create a window and renderer
    SDL_Window *window = initOpenGL(SCREEN_WIDTH,SCREEN_HEIGHT);

    GLuint basicCubeShader = loadDefaultShaders("graphics/shaders/vertex.glsl", "graphics/shaders/fragment.glsl");

    unsigned long long frequency = SDL_GetPerformanceFrequency();
    unsigned long long startTime = SDL_GetPerformanceCounter();
    int screenWidth = SCREEN_WIDTH, screenHeight = SCREEN_HEIGHT;

    bool mouseLocked = true;

    camera me(basicCubeShader);
    me.setAspectRatio(SCREEN_WIDTH,SCREEN_HEIGHT);

    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_FRONT);
    glActiveTexture(GL_TEXTURE0);

    // Load model:

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile("assets/blockOfAheago.fbx",aiProcess_Triangulate);
    if(!scene)
        std::cout<<"Could not load model!\n";
    else
    {
        std::cout<<"Loaded model!\n";
    }

    std::cout << "Model has following attributes: " << std::endl <<
        "HasAnimations: " << scene->HasAnimations() << std::endl <<
        "HasCameras(): " << scene->HasCameras() << std::endl <<
        "HasLights(): " << scene->HasLights() << std::endl <<
        "HasMaterials(): " << scene->HasMaterials() << std::endl <<
        "HasMeshes(): " << scene->HasMeshes() << std::endl <<
        "HasTextures(): " << scene->HasTextures() << std::endl;

    /*
     * I worry about not seeing a mesh on my FBX file, likely due to export settings within blender
     * I should only allow FBX files to render, as FBX files contain a material, a mesh, a texture hopefully too
     *
     * Do or should we care about lights? Im not sure.. We would want to allow for cool effects on things but i just dont know if lights are the way to do this
     *
     * First thing to ensure is that the mesh is wound properly, and i would only assume that we would already have the mesh wound properly by the way that blender exports it and assimp parses it
     * If thats not the case.. then we have a pretty big problem
     *
     * Objects should be a collection of meshes and moved as one, never separated
     *
     * how would you make a planet with a moon? that would have to be an animation and contain keyframes, which i can do later
     * 
     */
    // Build the triangle mesh

    // Decide the size of the triangle
    float vertices[] = {
            0.0f,  0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);


    // Enter the main loop

    while (!quit)
    {
        Uint64 currentTime = SDL_GetPerformanceCounter();
        double delta = ((double)(currentTime - startTime) / frequency) * 100;
        //double sinceStart = ((double)(currentTime - originTime) / frequency);
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
        glClearColor(0.21,0.66,1,0);
        //You can choose to clear only the color data, or the depth data, or both:
        //I.e. if you don't clear depth data, fragments will only render if they're in front of everything from last frame
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0,0,screenWidth,screenHeight);
        glBindFramebuffer(GL_FRAMEBUFFER,0);

        glm::mat4 identity = glm::mat4(1.0);
        glUniformMatrix4fv(glGetUniformLocation(basicCubeShader,"modelMatrix"),1,GL_FALSE,&identity[0][0]);

        glUseProgram(basicCubeShader);
        me.render();

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_GL_SwapWindow(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    //progressiveBuilder.join();
    return 0;
}