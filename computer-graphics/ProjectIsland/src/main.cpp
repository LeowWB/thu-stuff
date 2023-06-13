// Std. Includes
#include <string>
#include <map>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "shader.h"
#include "camera.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include <SOIL.h>

const float PI = acos(-1);

// Properties
GLuint WIDTH = 1800, HEIGHT = 1000;

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();
GLuint loadTexture(const char* path, GLboolean alpha = false);

// Camera
Camera camera(glm::vec3(0.0f, 1.0f, 0.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

// Water movement
float waterPhase = 0.0f;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// The MAIN function, from here we start our application and run our Game loop
int main()
{
    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Wandering Island", nullptr, nullptr); // Windowed
    glfwMakeContextCurrent(window);
    
    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    // Options
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Initialize GLAD to setup the OpenGL Function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // Setup some OpenGL options
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Setup and compile our shaders
    Shader shader("main.vert.glsl", "main.frag.glsl");
    Shader islandShader("island.vert.glsl", "island.frag.glsl");

    // Skybox
    const GLfloat SKYBOX_REL_HT = 1.5f;
    const GLfloat SKYBOX_SCALE = 500.0f;
    GLfloat skyboxNegXVerts[] = {
        -1.0, 0.0, -1.0, 0.0, 0.0,
        -1.0, SKYBOX_REL_HT, -1.0, 0.0, 1.0,
        -1.0, 0.0, 1.0, 1.0, 0.0,

        -1.0, SKYBOX_REL_HT, 1.0, 1.0, 1.0,
        -1.0, SKYBOX_REL_HT, -1.0, 0.0, 1.0,
        -1.0, 0.0, 1.0, 1.0, 0.0
    };
    GLfloat skyboxPosXVerts[] = {
        1.0, 0.0, -1.0, 1.0, 0.0,
        1.0, SKYBOX_REL_HT, -1.0, 1.0, 1.0,
        1.0, 0.0, 1.0, 0.0, 0.0,

        1.0, SKYBOX_REL_HT, 1.0, 0.0, 1.0,
        1.0, SKYBOX_REL_HT, -1.0, 1.0, 1.0,
        1.0, 0.0, 1.0, 0.0, 0.0
    };
    GLfloat skyboxNegZVerts[] = {
        -1.0, 0.0, -1.0, 1.0, 0.0,
        -1.0, SKYBOX_REL_HT, -1.0, 1.0, 1.0,
        1.0, 0.0, -1.0, 0.0, 0.0,

        1.0, SKYBOX_REL_HT, -1.0, 0.0, 1.0,
        -1.0, SKYBOX_REL_HT, -1.0, 1.0, 1.0,
        1.0, 0.0, -1.0, 0.0, 0.0
    };
    GLfloat skyboxPosZVerts[] = {
        -1.0, 0.0, 1.0, 0.0, 0.0,
        -1.0, SKYBOX_REL_HT, 1.0, 0.0, 1.0,
        1.0, 0.0, 1.0, 1.0, 0.0,

        1.0, SKYBOX_REL_HT, 1.0, 1.0, 1.0,
        -1.0, SKYBOX_REL_HT, 1.0, 0.0, 1.0,
        1.0, 0.0, 1.0, 1.0, 0.0
    };
    GLfloat skyboxTopVerts[] = {
        -1.0, SKYBOX_REL_HT, -1.0, 0.0, 0.0,
        1.0, SKYBOX_REL_HT, -1.0, 0.0, 1.0,
        -1.0, SKYBOX_REL_HT, 1.0, 1.0, 0.0,

        1.0, SKYBOX_REL_HT, 1.0, 1.0, 1.0,
        1.0, SKYBOX_REL_HT, -1.0, 0.0, 1.0,
        -1.0, SKYBOX_REL_HT, 1.0, 1.0, 0.0,
    };
    GLfloat skyboxBotVerts[] = {
        -1.0, 0.0, -1.0, SKYBOX_SCALE, SKYBOX_SCALE,
        1.0, 0.0, -1.0, SKYBOX_SCALE, 0.0,
        -1.0, 0.0, 1.0, 0.0, SKYBOX_SCALE,

        1.0, 0.0, 1.0, 0.0, 0.0,
        1.0, 0.0, -1.0, SKYBOX_SCALE, 0.0,
        -1.0, 0.0, 1.0, 0.0, SKYBOX_SCALE,
    };

    // Island
    const float ISLAND_REL_HT = 0.3f;
    const float ISLAND_SCALE = 0.1f;
    const float ISLAND_CUTOFF_Y = 1 / ISLAND_REL_HT / ISLAND_SCALE;
    int islandW = 0;
    int islandH = 0;
    unsigned char* depthMap = SOIL_load_image("heightmap.bmp", &islandW, &islandH, 0, SOIL_LOAD_RGB);
    GLfloat* islandVerts = new GLfloat[(islandW - 1) * (islandH - 1) * 2 * 15];
    int nextIslandVertInd = 0;
    for (int i = 0; i < islandW - 1; i++) {
        for (int j = 0; j < islandH - 1; j++) {
            islandVerts[nextIslandVertInd++] = GLfloat(i);
            islandVerts[nextIslandVertInd++] = GLfloat(depthMap[3 * (i + j * islandW)]);
            islandVerts[nextIslandVertInd++] = GLfloat(j);
            islandVerts[nextIslandVertInd++] = GLfloat(i) / GLfloat(islandW-1);
            islandVerts[nextIslandVertInd++] = 1 - GLfloat(j) / GLfloat(islandH - 1);

            islandVerts[nextIslandVertInd++] = GLfloat(i + 1);
            islandVerts[nextIslandVertInd++] = GLfloat(depthMap[3 * (i + 1 + j * islandW)]);
            islandVerts[nextIslandVertInd++] = GLfloat(j);
            islandVerts[nextIslandVertInd++] = GLfloat(i+1) / GLfloat(islandW - 1);
            islandVerts[nextIslandVertInd++] = 1 - GLfloat(j) / GLfloat(islandH - 1);

            islandVerts[nextIslandVertInd++] = GLfloat(i);
            islandVerts[nextIslandVertInd++] = GLfloat(depthMap[3 * (i + (j+1) * islandW)]);
            islandVerts[nextIslandVertInd++] = GLfloat(j+1);
            islandVerts[nextIslandVertInd++] = GLfloat(i) / GLfloat(islandW - 1);
            islandVerts[nextIslandVertInd++] = 1 - GLfloat(j+1) / GLfloat(islandH - 1);


            islandVerts[nextIslandVertInd++] = GLfloat(i + 1);
            islandVerts[nextIslandVertInd++] = GLfloat(depthMap[3*(i + 1 + j * islandW)]);
            islandVerts[nextIslandVertInd++] = GLfloat(j);
            islandVerts[nextIslandVertInd++] = GLfloat(i+1) / GLfloat(islandW - 1);
            islandVerts[nextIslandVertInd++] = 1 - GLfloat(j) / GLfloat(islandH - 1);

            islandVerts[nextIslandVertInd++] = GLfloat(i);
            islandVerts[nextIslandVertInd++] = GLfloat(depthMap[3*(i + (j + 1) * islandW)]);
            islandVerts[nextIslandVertInd++] = GLfloat(j + 1);
            islandVerts[nextIslandVertInd++] = GLfloat(i) / GLfloat(islandW - 1);
            islandVerts[nextIslandVertInd++] = 1 - GLfloat(j+1) / GLfloat(islandH - 1);

            islandVerts[nextIslandVertInd++] = GLfloat(i+1);
            islandVerts[nextIslandVertInd++] = GLfloat(depthMap[3*(i + 1 + (j+1) * islandW)]);
            islandVerts[nextIslandVertInd++] = GLfloat(j+1);
            islandVerts[nextIslandVertInd++] = GLfloat(i + 1) / GLfloat(islandW - 1);
            islandVerts[nextIslandVertInd++] = 1 - GLfloat(j + 1) / GLfloat(islandH - 1);
        }
    }

    // Set the object data (buffers, vertex attributes)
    
    // Skybox VAOs
    GLuint sbnxVAO, sbnxVBO;
    glGenVertexArrays(1, &sbnxVAO);
    glGenBuffers(1, &sbnxVBO);
    glBindVertexArray(sbnxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sbnxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxNegXVerts), &skyboxNegXVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glBindVertexArray(0);

    GLuint sbpxVAO, sbpxVBO;
    glGenVertexArrays(1, &sbpxVAO);
    glGenBuffers(1, &sbpxVBO);
    glBindVertexArray(sbpxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sbpxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxPosXVerts), &skyboxPosXVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glBindVertexArray(0);

    GLuint sbnzVAO, sbnzVBO;
    glGenVertexArrays(1, &sbnzVAO);
    glGenBuffers(1, &sbnzVBO);
    glBindVertexArray(sbnzVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sbnzVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxNegZVerts), &skyboxNegZVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glBindVertexArray(0);

    GLuint sbpzVAO, sbpzVBO;
    glGenVertexArrays(1, &sbpzVAO);
    glGenBuffers(1, &sbpzVBO);
    glBindVertexArray(sbpzVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sbpzVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxPosZVerts), &skyboxPosZVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glBindVertexArray(0);

    GLuint sbtVAO, sbtVBO;
    glGenVertexArrays(1, &sbtVAO);
    glGenBuffers(1, &sbtVBO);
    glBindVertexArray(sbtVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sbtVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxTopVerts), &skyboxTopVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glBindVertexArray(0);

    GLuint sbbVAO, sbbVBO;
    glGenVertexArrays(1, &sbbVAO);
    glGenBuffers(1, &sbbVBO);
    glBindVertexArray(sbbVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sbbVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxBotVerts), &skyboxBotVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glBindVertexArray(0);
    
    // Island
    GLuint islandVAO, islandVBO;
    glGenVertexArrays(1, &islandVAO);
    glGenBuffers(1, &islandVBO);
    glBindVertexArray(islandVAO);
    glBindBuffer(GL_ARRAY_BUFFER, islandVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * (islandW - 1) * (islandH - 1) * 2 * 15, islandVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glBindVertexArray(0);


    // Load textures
    GLuint sbnxTexture = loadTexture("SkyBox0.bmp");
    GLuint sbpzTexture = loadTexture("SkyBox1.bmp");
    GLuint sbpxTexture = loadTexture("SkyBox2.bmp");
    GLuint sbnzTexture = loadTexture("SkyBox3.bmp");
    GLuint sbtTexture = loadTexture("SkyBox4.bmp");
    GLuint sbbTexture = loadTexture("SkyBox5.png", true);
    GLuint islandTexture = loadTexture("terrain-texture3.bmp");
    GLuint noiseTexture = loadTexture("detail.bmp");
    
    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        waterPhase += deltaTime * 0.2;
        
        glEnable(GL_CLIP_DISTANCE0);

        // Check and call events
        glfwPollEvents();
        Do_Movement();
        
        // Clear the colorbuffer
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Draw objects
        shader.Use();
        glm::mat4 model(1);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 2000.0f);
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        
        // Skybox
        glBindVertexArray(sbnxVAO);
        glBindTexture(GL_TEXTURE_2D, sbnxTexture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        model = glm::mat4(1);
        model = glm::scale(model, SKYBOX_SCALE * glm::vec3(1, 1, 1));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(sbpxVAO);
        glBindTexture(GL_TEXTURE_2D, sbpxTexture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        model = glm::mat4(1);
        model = glm::scale(model, SKYBOX_SCALE * glm::vec3(1, 1, 1));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(sbnzVAO);
        glBindTexture(GL_TEXTURE_2D, sbnzTexture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        model = glm::mat4(1);
        model = glm::scale(model, SKYBOX_SCALE * glm::vec3(1, 1, 1));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(sbpzVAO);
        glBindTexture(GL_TEXTURE_2D, sbpzTexture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        model = glm::mat4(1);
        model = glm::scale(model, SKYBOX_SCALE * glm::vec3(1, 1, 1));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(sbtVAO);
        glBindTexture(GL_TEXTURE_2D, sbtTexture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        model = glm::mat4(1);
        model = glm::scale(model, SKYBOX_SCALE * glm::vec3(1, 1, 1));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Skybox reflection

        glBindVertexArray(sbnxVAO);
        glBindTexture(GL_TEXTURE_2D, sbnxTexture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        model = glm::mat4(1);
        model = glm::scale(model, SKYBOX_SCALE * glm::vec3(1, 1, 1));
        model = glm::translate(model, glm::vec3(-2, 0, 0));
        model = glm::rotate(model, PI, glm::vec3(0, 0, 1));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(sbpxVAO);
        glBindTexture(GL_TEXTURE_2D, sbpxTexture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        model = glm::mat4(1);
        model = glm::scale(model, SKYBOX_SCALE * glm::vec3(1, 1, 1));
        model = glm::translate(model, glm::vec3(2, 0, 0));
        model = glm::rotate(model, PI, glm::vec3(0, 0, 1));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(sbnzVAO);
        glBindTexture(GL_TEXTURE_2D, sbnzTexture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        model = glm::mat4(1);
        model = glm::scale(model, SKYBOX_SCALE * glm::vec3(1, 1, 1));
        model = glm::translate(model, glm::vec3(0, 0, -2));
        model = glm::rotate(model, PI, glm::vec3(1, 0, 0));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(sbpzVAO);
        glBindTexture(GL_TEXTURE_2D, sbpzTexture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        model = glm::mat4(1);
        model = glm::scale(model, SKYBOX_SCALE * glm::vec3(1, 1, 1));
        model = glm::translate(model, glm::vec3(0, 0, 2));
        model = glm::rotate(model, PI, glm::vec3(1, 0, 0));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(sbtVAO);
        glBindTexture(GL_TEXTURE_2D, sbtTexture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        model = glm::mat4(1);
        model = glm::scale(model, SKYBOX_SCALE * glm::vec3(1, 1, 1));
        model = glm::translate(model, glm::vec3(0, -2 * SKYBOX_REL_HT, 0));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Island reflection
        islandShader.Use();
        glActiveTexture(GL_TEXTURE0);
        glTexEnvi(GL_TEXTURE0, GL_ADD_SIGNED, 1);
        glBindTexture(GL_TEXTURE_2D, islandTexture);
        glUniform1i(glGetUniformLocation(islandShader.Program, "tex1"), 0);
        glActiveTexture(GL_TEXTURE1);
        glTexEnvi(GL_TEXTURE1, GL_ADD_SIGNED, 0.1);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        glUniform1i(glGetUniformLocation(islandShader.Program, "tex2"), 1);
        glUniform1i(glGetUniformLocation(islandShader.Program, "cutoff"), ISLAND_CUTOFF_Y * 10 * ISLAND_REL_HT);
        glUniform1i(glGetUniformLocation(islandShader.Program, "direction"), 1);
        glUniformMatrix4fv(glGetUniformLocation(islandShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(islandShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glBindVertexArray(islandVAO);
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(0, 10 * ISLAND_REL_HT, 0));
        model = glm::scale(model, ISLAND_SCALE * glm::vec3(1, -ISLAND_REL_HT, 1));
        glUniformMatrix4fv(glGetUniformLocation(islandShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 6 * (islandW - 1) * (islandH - 1));
        shader.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(0);


        // Waves (skybox bottom) (render from bottom-up)

        glBindVertexArray(sbbVAO);
        glBindTexture(GL_TEXTURE_2D, sbbTexture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(0, -0.01, cos(waterPhase)));
        model = glm::scale(model, SKYBOX_SCALE * glm::vec3(1, 1, 1));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(sbbVAO);
        glBindTexture(GL_TEXTURE_2D, sbbTexture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(sin(waterPhase), 0, 0));
        model = glm::scale(model, SKYBOX_SCALE * glm::vec3(1, 1, 1));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Island
        islandShader.Use();
        glActiveTexture(GL_TEXTURE0);
        glTexEnvi(GL_TEXTURE0, GL_ADD_SIGNED, 1);
        glBindTexture(GL_TEXTURE_2D, islandTexture);
        glUniform1i(glGetUniformLocation(islandShader.Program, "tex1"), 0);
        glActiveTexture(GL_TEXTURE1);
        glTexEnvi(GL_TEXTURE1, GL_ADD_SIGNED, 0.1);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        glUniform1i(glGetUniformLocation(islandShader.Program, "tex2"), 1);
        glUniform1i(glGetUniformLocation(islandShader.Program, "cutoff"), ISLAND_CUTOFF_Y * 10 * ISLAND_REL_HT);
        glUniform1i(glGetUniformLocation(islandShader.Program, "direction"), 1);
        glUniformMatrix4fv(glGetUniformLocation(islandShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(islandShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glBindVertexArray(islandVAO);
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(0, -10 * ISLAND_REL_HT, 0));
        model = glm::scale(model, ISLAND_SCALE * glm::vec3(1, ISLAND_REL_HT, 1));
        glUniformMatrix4fv(glGetUniformLocation(islandShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 6 * (islandW-1) * (islandH-1));
        shader.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(0);


        // Swap the buffers
        glfwSwapBuffers(window);
    }
    
    glfwTerminate();
    return 0;
}

// This function loads a texture from file. Note: texture loading functions like these are usually
// managed by a 'Resource Manager' that manages all resources (like textures, models, audio).
// For learning purposes we'll just define it as a utility function.
GLuint loadTexture(const char* path, GLboolean alpha)
{
    //Generate texture ID and load texture data
    GLuint textureID;
    glGenTextures(1, &textureID);
    int width,height;
    unsigned char* image = SOIL_load_image(path, &width, &height, 0, alpha ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);
    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, alpha ? GL_RGBA : GL_RGB, width, height, 0, alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // Parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT );    // Use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes value from next repeat
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image);
    return textureID;
}


// Moves/alters the camera positions based on user input
void Do_Movement()
{
    // Camera controls
    if (keys[GLFW_KEY_W])
    camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S])
    camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A])
    camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D])
    camera.ProcessKeyboard(RIGHT, deltaTime);
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
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
        keys[key] = true;
        else if (action == GLFW_RELEASE)
        keys[key] = false;
    }
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;
    
    lastX = xpos;
    lastY = ypos;
    
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

