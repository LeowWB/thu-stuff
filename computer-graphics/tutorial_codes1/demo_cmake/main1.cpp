#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

int main(int argc, const char *argv[])
{
    glfwInit(); // init glfw
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // configure glfw
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // args are option name, option value
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "fail to create window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // set window context to be main context on cur thread
    glfwMakeContextCurrent(window);

    // pass glad a func which returns the ptrs to the opengl funcs on the os.
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // first 2 params: lower left corner coords. other 2 set height, width.
    // if viewport size smaller than window size, we can have multiple viewports in one window.
    glViewport(0, 0, 800, 600);

    // setup callback function to be activated whenever window is resized. func just resizes viewport.
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // render loop. this part keeps looping indefinitely, drawing images etc until told to close
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // set a color to clear the screen
        glClear(GL_COLOR_BUFFER_BIT); // clear the screen w that color.

        // swap the color buffer that's being used to draw for cur iteration, and display on screen
        // the old buffer is then used for drawing for the next iter
        glfwSwapBuffers(window);
        
        // check if any events are triggered, upd8 window st8, call corresponding callbacks
        // example of callback: framebuffer_size_callback (seen right before the loop)
        glfwPollEvents();
    }

    glfwTerminate(); // release resources
    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}