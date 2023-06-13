#include <iostream>

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

// Other includes
#include "shader.h"
#include "ParticleSystem.h"


// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Window dimensions
GLuint WIDTH = 800, HEIGHT = 600;
// Deltatime
GLfloat deltaTime = 0.0f;    // Time between current frame and last frame
GLfloat lastFrame = 0.0f;      // Time of last frame
FireWork **fireWorks;
const int fireWorkNum = 3;


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
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Particles", nullptr, nullptr);
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
    
    
    fireWorks = new FireWork* [fireWorkNum];
    for (int i = 0; i < fireWorkNum; ++i)
    {
        fireWorks[i] = new FireWork(500, 0.5, glm::vec3(rand()%(WIDTH/3)+WIDTH/3*i, HEIGHT/4, 0),
                                    glm::vec3(0.0, 70, 0.0), glm::vec3(0.0, -9.8, 0.0), (double)rand() / (double)RAND_MAX * 2 + 12);
        // massNum: 500, each firework consists of 500 particle
        // mass: 5, each particle's mass is 5
        // position: start position
        // speed: initial speed is (0, 70, 0)
        // gravity: gravity is (0, -9.8, 0)
        // lifeValue: existing time in seconds
    }

    
    
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
    unsigned char* image = SOIL_load_image("Particle.bmp", &width, &height, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
    
    
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
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        for (int i = 0; i < fireWorkNum; ++i)
            fireWorks[i]->process(deltaTime*3, glm::vec3(rand()%(WIDTH/3)+WIDTH/3*i, HEIGHT/4, 0)); // apply gravity and update speed, position
        
        // Draw
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glm::mat4 projection = glm::ortho(0.0f, GLfloat(WIDTH), 0.0f, GLfloat(HEIGHT), -1.0f, 100.0f);
        particleShader.Use();
        GLint projLoc = glGetUniformLocation(particleShader.Program, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        for (int i = 0; i < fireWorkNum; ++i)
        {
            if(fireWorks[i]->hasExploded)
            {
                for (int j = 0;j < fireWorks[i]->getMassNum();++j){
                    GLint offsetLoc = glGetUniformLocation(particleShader.Program, "offset");
                    glUniform2f(offsetLoc, fireWorks[i]->getMass(j)->position[0], fireWorks[i]->getMass(j)->position[1]);
                    GLint colorLoc = glGetUniformLocation(particleShader.Program, "color");
                    glUniform4f(colorLoc, fireWorks[i]->getMass(j)->color.r, fireWorks[i]->getMass(j)->color.g, fireWorks[i]->getMass(j)->color.b,
                                fireWorks[i]->getMass(j)->color.a);
                    glBindTexture(GL_TEXTURE_2D, texture);
                    glBindVertexArray(VAO);
                    glDrawArrays(GL_TRIANGLES, 0, 6);
                    glBindVertexArray(0);
                }
            }
            else{// before explosion, only one point
                int j = 0;
                GLint offsetLoc = glGetUniformLocation(particleShader.Program, "offset");
                glUniform2f(offsetLoc, fireWorks[i]->getMass(j)->position[0], fireWorks[i]->getMass(j)->position[1]);
                GLint colorLoc = glGetUniformLocation(particleShader.Program, "color");
                glUniform4f(colorLoc, fireWorks[i]->getMass(j)->color.r, fireWorks[i]->getMass(j)->color.g, fireWorks[i]->getMass(j)->color.b,
                            fireWorks[i]->getMass(j)->color.a);
                glBindTexture(GL_TEXTURE_2D, texture);
                glBindVertexArray(VAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                glBindVertexArray(0);
            }
        }
        
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

