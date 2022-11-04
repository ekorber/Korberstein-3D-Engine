#include "InputControls.h"
#include <GLFW/glfw3.h>

bool leftPressed = false;
bool rightPressed = false;
bool upPressed = false;
bool downPressed = false;

// Change input button boolean value if button pressed, or released.
// Serves as a 'hold down button' function.
void setInputValue(bool& inputButton, int action) {

    if (action == GLFW_PRESS) {
        inputButton = true;
    }
    else if (action == GLFW_RELEASE) {
        inputButton = false;
    }
}

void handleInput(int key, int action)
{
    switch (key)
    {
    case GLFW_KEY_W:
        setInputValue(upPressed, action);
        break;
    case GLFW_KEY_A:
        setInputValue(leftPressed, action);
        break;
    case GLFW_KEY_S:
        setInputValue(downPressed, action);
        break;
    case GLFW_KEY_D:
        setInputValue(rightPressed, action);
        break;

    default:
        break;
    }
}