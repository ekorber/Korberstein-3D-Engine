#include <GLFW/glfw3.h>
#include "Controls/InputControls.h"
#include "Graphics/Raycast3DEngine.h"

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    handleInput(key, action);

    //Set window to close if Esc is pressed
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    int screenWidth = 600;
    int screenHeight = 600;

    // Prevent user from manually resizing the window
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(screenWidth, screenHeight, "My Fancy 3D Engine", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Set keyboard input callback function */
    glfwSetKeyCallback(window, key_callback);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Initialize renderer */
    raycastRendererInit(screenWidth, screenHeight);

    float lastFrameTime = 0;
    float currentFrameTime;
    float deltaTime;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Clear screen */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Calculate time since last frame */
        currentFrameTime = glfwGetTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        /* Render the screen */
        raycastRenderLoop(deltaTime);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}