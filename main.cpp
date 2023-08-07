#define SDL_MAIN_HANDLED

#include "graphics/Camera.h"
#include "graphics/Shader.h"
#include "graphics/Model.h"
#include <glm/gtx/quaternion.hpp>
#include "halcyonicus.h"
#include "lib/crypto/ecdh.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

Model ourModel("assets/ahe.fbx");

float rx = 0, ry = 0, rz = 0;



int main(int argc, char* argv[])
{


    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif


    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);

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

    // build and compile shaders
    // -------------------------
    Shader ourShader("graphics/shaders/1.model_loading.vs", "graphics/shaders/1.model_loading.fs");

    // load models
    // -----------
    ourModel.Load();


    std::vector<glm::mat4> modelMatricies;
    std::vector<glm::vec3> greenishness;
    for(int a = 0; a<100; a++)
    {
        modelMatricies.push_back(glm::translate(glm::vec3(rand() % 100,rand() % 100,rand() % 100)));
        float greenish = rand() % 100;
        greenish /= 100.0;
        greenishness.push_back(glm::vec3(0.5,greenish,0.5));
    }


    // draw in wireframe
   // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    float rotation = 0;
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.3f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the loaded model
        /*
        glm::mat4 model = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        model = glm::toMat4(glm::quat(1.0,0.0,0.0,0.0)) * model;
        model = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f)) * model; // translate it down so it's at the center of the scene
        ourShader.setMat4("model", model);

        ourModel.Draw(ourShader);
        */
        // render the loaded model
        rotation += deltaTime;

        glm::mat4 model = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down

        model = glm::toMat4(glm::quat(glm::vec3(rx, ry, rz))) * model;



        model = glm::translate(glm::vec3(0.0f, 5.0f, 0.0F)) * model; // translate it down so it's at the center of the scene
        ourShader.setMat4("model", model);

        ourModel.Draw(ourShader);

        for(int a = 0; a<modelMatricies.size(); a++)
        {
            ourShader.setVec3("greenish",greenishness[a]);
            ourShader.setMat4("model",modelMatricies[a]);
            ourModel.Draw(ourShader);
        }


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
