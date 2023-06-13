// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include <SOIL.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <queue>

// Other includes
#include "shader.h"

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Window dimensions
GLuint WIDTH = 800, HEIGHT = 600;
// Deltatime
GLfloat deltaTime = 0.0f;    // Time between current frame and last frame
GLfloat lastFrame = 0.0f;      // Time of last frame

struct snowflake {
    GLfloat timeAlive;
    GLfloat xPos;
    GLfloat speed;
    GLfloat size;
};

std::queue<snowflake> snowflakes;
float timeToNextSnowflake = 1.0f;
float lastSnowflakeTime = 1.0f;

// The MAIN function, from here we start the application and run the game loop
int main()
{
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Snow", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    
    // Set the required callback functions
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    
    // Initialize GLAD to setup the OpenGL Function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // Build and compile our shader program
    Shader particleShader("main.vert.glsl", "main.frag.glsl");
    
    // Set up mesh and attribute properties
    GLuint VBO, VAO;
    GLfloat particle_quad[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    // Fill mesh buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
    // Set mesh attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0);
    
    
    // Load and create a texture
    GLuint texture;
    // ====================
    // Texture
    // ====================
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
    // Set our texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    // Set texture wrapping to GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load, create texture and generate mipmaps
    int width, height;
    unsigned char* image = SOIL_load_image("snow2.png", &width, &height, 0, SOIL_LOAD_RGBA);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
    
    // bg txture
    GLuint bgTex;
    glGenTextures(1, &bgTex);
    glBindTexture(GL_TEXTURE_2D, bgTex); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    // Set texture wrapping to GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int bgwidth, bgheight;
    unsigned char* bg = SOIL_load_image("bg.png", &bgwidth, &bgheight, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bgwidth, bgheight, 0, GL_RGB, GL_UNSIGNED_BYTE, bg);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(bg);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

    glm::mat4 model(1);

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Calculate deltatime of current frame
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        
        // Render
        // Clear the colorbuffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Process
        timeToNextSnowflake -= deltaTime;

        if (timeToNextSnowflake < 0.0f) {
            lastSnowflakeTime = std::max(0.25f, lastSnowflakeTime - 0.05f);
            timeToNextSnowflake = lastSnowflakeTime;

            snowflake sf;
            sf.timeAlive = 0.0f;
            sf.xPos = float(std::rand() % (WIDTH));
            sf.size = float(std::rand()) / float(RAND_MAX) * 10.0f;
            sf.speed = float(std::rand()) / float(RAND_MAX) * 50.0f + 15.0f;

            snowflakes.push(sf);
        }

        for (int i = 0; i < snowflakes.size(); i++) {
            snowflake sf = snowflakes.front();
            snowflakes.pop();
            sf.timeAlive += deltaTime;
            
            if (sf.timeAlive <= 50.0f) // height is 600, min speed is 15. so 40 seconds should be enough.
                snowflakes.push(sf);
        }

        // Draw
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glm::mat4 projection = glm::ortho(GLfloat(-0.5 * WIDTH), GLfloat(0.5 * WIDTH), GLfloat(-0.5 * HEIGHT), GLfloat(0.5 * HEIGHT), -1.0f, 100.0f);
        particleShader.Use();
        GLint projLoc = glGetUniformLocation(particleShader.Program, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        for (int i = 0; i < snowflakes.size(); ++i)
        {
            snowflake sf = snowflakes.front();
            snowflakes.pop();
            snowflakes.push(sf);

            model = glm::mat4(1);
            model = glm::translate(model, glm::vec3(sf.xPos - float(WIDTH)/2.0f, -(sf.timeAlive * sf.speed) + 0.5f * (float(HEIGHT)), 0.0f));
            model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f) * (sf.size));
            glUniformMatrix4fv(glGetUniformLocation(particleShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

            GLint colorLoc = glGetUniformLocation(particleShader.Program, "color");
            glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 0.5f);

            glBindTexture(GL_TEXTURE_2D, texture);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }

        // draw bg (using the particle shader so we don't have to make a new shader)
        GLint colorLoc = glGetUniformLocation(particleShader.Program, "color");
        glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(- (float)(WIDTH) / 2.0f, (float)(HEIGHT) / 2.0f, 0.0f));
        model = glm::scale(model, glm::vec3((float)(WIDTH) / 5.0f, -(float)(HEIGHT) / 5.0f, 1.0f));
        glUniformMatrix4fv(glGetUniformLocation(particleShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glBindTexture(GL_TEXTURE_2D, bgTex);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    WIDTH = width;
    HEIGHT = height;
    glViewport(0, 0, width, height);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

