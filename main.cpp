#define SDL_MAIN_HANDLED
#define NOMINMAX

#include <glm/gtx/quaternion.hpp>

#include "graphics/Camera.h"
#include "graphics/Shader.h"
#include "graphics/Model.h"

#include "lib/logging/hlogger.h"

#include <cmath>
#include "lib/noise/perlin.h"
#include "lib/world/chunk.h"
#include "lib/world/generator.h"
#include "graphics/chunk/chunk3d.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool mouseAttached = true;
bool maOneshot = false;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

Model ourModel("assets/ahe.fbx");

float rx = 0, ry = 0, rz = 0;

std::vector<GLfloat> cubeVertices = {
        // Front face
        -0.5f, -0.5f,  0.5f, // Vertex 0
        0.5f, -0.5f,  0.5f, // Vertex 1
        0.5f,  0.5f,  0.5f, // Vertex 2
        -0.5f,  0.5f,  0.5f, // Vertex 3

        // Back face
        -0.5f, -0.5f, -0.5f, // Vertex 4
        0.5f, -0.5f, -0.5f, // Vertex 5
        0.5f,  0.5f, -0.5f, // Vertex 6
        -0.5f,  0.5f, -0.5f  // Vertex 7
};

// Indices to render the cube as a set of triangles
std::vector<GLuint> cubeIndices ={
        // Front face
        0, 1, 2,
        2, 3, 0,

        // Right face
        1, 5, 6,
        6, 2, 1,

        // Back face
        5, 4, 7,
        7, 6, 5,

        // Left face
        4, 0, 3,
        3, 7, 4,

        // Top face
        3, 2, 6,
        6, 7, 3
};

using namespace hlogger;
int main(int argc, char* argv[])
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    //Does not work for no good reason
    //_setmode(_fileno(stdout), _O_U16TEXT);
    // Enable buffering to prevent VS from chopping up UTF-8 byte sequences
    setvbuf(stdout, nullptr, _IOFBF, 2000);
#endif
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "JustACubeTest", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // Enable backface culling
    glEnable(GL_CULL_FACE);

    // Optionally, specify which faces to cull (back faces by default)
    glCullFace(GL_BACK);

    // build and compile shaders
    // -------------------------
    Shader ourShader("graphics/shaders/1.model_loading.vs", "graphics/shaders/1.model_loading.fs");

    // load models
    // -----------
    //ourModel.Load();

    // Our chunk here:
    // ---------------

    std::vector<CHUNK*> chunks;

    for (short x = -2; x < 2; x++) {
        for (short y = -2; y < 2; y++) {
            chunks.push_back(new CHUNK({x, y}));
        }
    }

    for (auto c : chunks){
        GENERATOR::genHeight(c, 0, 8008);
        GENERATOR::genHumidity(c, 8008);
    }

    for (auto & x : chunks.at(0)->humidity) {
        for (unsigned char y : x) {
            std::cout << (int)y << " ";
        }
        std::cout << nl;
    }

    info << "Setting up meshes.." << nl;
    std::vector<CHUNK3D*> renders;
    renders.reserve(chunks.size());

    for (auto c : chunks){
        renders.push_back(new CHUNK3D(c));
    }
    for(auto c : renders){
        c->setup();
    }

    // draw in wireframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    /*
    // Create and bind VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create and bind VBO for vertex data
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribDivisor(0,0);

    // Upload vertex data to the VBO
    glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(GLfloat), cubeVertices.data(), GL_STATIC_DRAW);

    // Create and bind EBO for indices
    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    // Upload index data to the EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndices.size() * sizeof(GLuint), cubeIndices.data(), GL_STATIC_DRAW);

    // Set up vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);

    // Hopefully make another thing that tells us the location of the blocks, just use a vec3 l
    GLuint vbo2;
    glGenBuffers(1, &vbo2);
    glBindBuffer(GL_ARRAY_BUFFER, vbo2);
    glVertexAttribDivisor(1,1);
    short* height = &c->layers[0].heights[0][0];
    glBufferData(GL_ARRAY_BUFFER, sizeof(c->layers[0].heights), height, GL_STATIC_DRAW);

    glVertexAttribIPointer(1, 1, GL_SHORT, 0, nullptr);
    glEnableVertexAttribArray(1);

    // Unbind VAO, VBO, and EBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
*/

    // render loop
    // -----------
    float counter = 0;
    int frames = 0;

    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (counter < 1) {
            counter += deltaTime;
            frames++;
        } else {
            std::cout << frames << nl;
            frames = 0;
            counter = 0.0;
        }
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0x00, 0x80, 0x7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        for (auto c : renders) {
            c->draw(view, projection, &ourShader);
        }
        /*glBindVertexArray(vao);
        // don't forget to enable shader before setting uniforms
        ourShader.use();

        // view/projection transformations

        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        glm::mat4 model = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));	// scale it
        model = glm::toMat4(glm::quat(1.0,0.0,0.0,0.0)) * model; // Rotate it
        model = glm::translate(glm::vec3(0.0, 0.0, 0.0)) * model; // Move it
        ourShader.setMat4("model", model);

        // Render the cube using glDrawElements
        glDrawElementsInstanced(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_INT, nullptr, 4096);


        // Unbind VAO
        glBindVertexArray(0);
*/

        //for(int a = 0; a<modelMatricies.size(); a++){

            /*
            siv::PerlinNoise noise0 {seeds[a]+6};
            ourShader.setFloat("lightness",(float) noise0.noise1D_01(lastFrame));
            glm::mat4 model = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));	// scale it
            model = glm::toMat4(glm::quat(1.0,0.0,0.0,0.0)) * model; // Rotate it
            //IDK if i need to do that above but  i dee kay!!! :DD
            siv::PerlinNoise noise {seeds[a]};
            siv::PerlinNoise noise2 {seeds[a]+2};
            siv::PerlinNoise noise3 {seeds[a]+4};
            model = glm::translate(glm::vec3(noise.noise1D(lastFrame), noise2.noise1D(lastFrame), noise3.noise1D(lastFrame))) * model; // Move it
            ourShader.setMat4("model",modelMatricies[a] * model);
            ourModel.Draw(ourShader);
             */
        //}

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.ProcessKeyboard(CTRLDOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)
        camera.ProcessKeyboard(CTRLUP, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(SHIFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(SPACE, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS){
        if (maOneshot) return;
        if (!mouseAttached){
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            mouseAttached = true;
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            mouseAttached = false;
        }
        maOneshot = true;

    }

    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_RELEASE){
        maOneshot = false;
    }


    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) // affects the vertical look direction
        rx += deltaTime;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) // affects the rolling look direction
        rz -= deltaTime;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) // affects the vertical look direction
        rx -= deltaTime;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) // affects the rolling look direction
        rz += deltaTime;
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) // affects the horizontal look direction
        ry += deltaTime;
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) // affects the horizontal look direction
        ry -= deltaTime;
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS){
        ry = 0;
        rx = 0;
        rz = 0;
    }

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    if (!mouseAttached) return;
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
