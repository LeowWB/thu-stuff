#include <iostream>
#include <cmath>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes
#include "shader.h"
#include "camera.h"


// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void do_movement();

// Window dimensions
GLuint WIDTH = 800, HEIGHT = 600;

// Camera
Camera  camera(glm::vec3(-1.0f, 1.3f, 1.2f), glm::vec3(0.0f, 1.0f, 0.0f), -57.0f, -22.0f);
GLfloat lastX  =  WIDTH  / 2.0;
GLfloat lastY  =  HEIGHT / 2.0;
bool    keys[1024];
bool firstMouse = true;

// Light attributes
glm::vec3 lightPos(0.8f, 1.0f, -0.2f);

// Deltatime
GLfloat deltaTime = 0.0f;    // Time between current frame and last frame
GLfloat lastFrame = 0.0f;      // Time of last frame

// handle material and lights
int curMat = 0;
int curLight = 3;
GLfloat materials[] = {
    1.0f, 0.5f, 0.31f, 1.0f, 0.5f, 0.31f, 0.5f, 0.5f, 0.5f, 16.0f,
    0.31f, 0.5f, 1.0f, 0.31f, 0.5f, 1.0f, 0.8f, 0.8f, 0.8f, 50.0f,
    0.31f, 0.31f, 0.31f, 0.31f, 0.31f, 0.31f, 0.2f, 0.2f, 0.2f, 5.0f
};

GLfloat lights[] = {
    1.0f, 0.2f, 0.2f,
    0.2f, 1.0f, 0.2f,
    0.2f, 0.2f, 1.0f,
    1.0f, 1.0f, 1.0f
};

// Set up vertex data (and buffer(s)) and attribute pointers
GLfloat cubeVertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
    0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
    0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
    0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

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
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Light2", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    
    // Set the required callback functions
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    // GLFW Options
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Initialize GLAD to setup the OpenGL Function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // OpenGL options
    glEnable(GL_DEPTH_TEST);
    
    // Build and compile our shader program
    Shader lightingShader("material.vert.glsl", "material.frag.glsl");
    Shader lampShader("lamp.vert.glsl", "lamp.frag.glsl");
    
    // eight

    std::ifstream obj_file;
    std::string next_line;
    obj_file.open("eight.uniform.obj");

    int num_vertices;
    int num_faces;
    GLfloat* vertex_defs = new GLfloat[18];
    GLfloat* vertices = new GLfloat[18];
    int next_vertex_def_ind = 0;
    int next_vertex_ind = 0;

    if (obj_file.is_open()) {
        while (obj_file) {
            std::getline(obj_file, next_line);

            if (next_line[0] == '#') {
                int last_space = next_line.find_last_of(' ');
                if (next_line[2] == 'v') {
                    num_vertices = std::stoi(next_line.substr(last_space));
                    vertex_defs = new GLfloat[num_vertices * 3];
                    std::cout << "Num vertices " << num_vertices << std::endl;
                }
                else if (next_line[2] == 'f') {
                    num_faces = std::stoi(next_line.substr(last_space));
                    vertices = new GLfloat[num_faces * 18];
                    std::cout << "Num faces " << num_faces << std::endl;
                }
            }
            else if (next_line[0] == 'v') {
                std::string p1;
                std::string p2;
                std::string p3;
                std::stringstream str_stream(next_line);
                std::getline(str_stream, p1, ' '); // skip cuz of the 'v'
                std::getline(str_stream, p1, ' ');
                std::getline(str_stream, p2, ' ');
                std::getline(str_stream, p3, ' ');
                vertex_defs[next_vertex_def_ind++] = std::stof(p1);
                vertex_defs[next_vertex_def_ind++] = std::stof(p2);
                vertex_defs[next_vertex_def_ind++] = std::stof(p3);
            }
            else if (next_line[0] == 'f') {
                // vertices are stored as 3 consec points in vertex_defs array
                std::string ps[3];
                std::stringstream str_stream(next_line);
                std::getline(str_stream, ps[0], ' '); // skip cuz of the 'f'
                std::getline(str_stream, ps[0], ' ');
                std::getline(str_stream, ps[1], ' ');
                std::getline(str_stream, ps[2], ' ');

                GLfloat vec1[3];
                GLfloat vec2[3];
                GLfloat crossProd[3];
                GLfloat crossProdMag = 0;

                for (int i = 0; i < 3; i++) {
                    int next_v_def_ind = std::stoi(ps[i]) - 1;
                    for (int j = 0; j < 3; j++) {
                        vertices[next_vertex_ind++] = vertex_defs[3 * next_v_def_ind + j];
                    }
                    next_vertex_ind += 3;
                }

                next_vertex_ind -= 18;

                for (int i = 0; i < 3; i++) {
                    vec1[i] = vertices[next_vertex_ind + i] - vertices[next_vertex_ind + 6 + i];
                    vec2[i] = vertices[next_vertex_ind + i] - vertices[next_vertex_ind + 12 + i];
                }

                crossProd[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
                crossProd[1] = vec1[2] * vec2[0] - vec1[0] * vec2[2];
                crossProd[2] = vec1[0] * vec2[1] - vec1[1] * vec2[0];
                
                for (int i = 0; i < 3; i++) {
                    crossProdMag += pow(crossProd[i], 2);
                }
                crossProdMag = pow(crossProdMag, 0.5); 

                for (int i = 0; i < 3; i++) {
                    crossProd[i] /= crossProdMag;
                }

                for (int i = 0; i < 3; i++) {
                    next_vertex_ind += 3;
                    for (int j = 0; j < 3; j++) {
                        vertices[next_vertex_ind++] = crossProd[j];
                    }
                }
            }
        }
    }
    else {
        std::cout << "error when opening file, file not open";
    }

    GLuint VBO, containerVAO;
    glGenVertexArrays(1, &containerVAO);
    glGenBuffers(1, &VBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * num_faces * 18, vertices, GL_STATIC_DRAW);
    
    glBindVertexArray(containerVAO);
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    

    // light
    GLuint lightVAO, lightVBO;
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightVBO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glBindVertexArray(lightVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Calculate deltatime of current frame
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        do_movement();
        
        // Clear the colorbuffer
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Use cooresponding shader when setting uniforms/drawing objects
        lightingShader.Use();
        GLint lightPosLoc    = glGetUniformLocation(lightingShader.Program, "light.position");
        GLint viewPosLoc     = glGetUniformLocation(lightingShader.Program, "viewPos");
        glUniform3f(lightPosLoc,    lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(viewPosLoc,     camera.Position.x, camera.Position.y, camera.Position.z);
        
        glm::vec3 lightColor;
        lightColor.x = lights[curLight * 3 + 0];
        lightColor.y = lights[curLight * 3 + 1];
        lightColor.z = lights[curLight * 3 + 2];
        glm::vec3 diffuseColor = lightColor   * glm::vec3(0.5f); // decrease the influence
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
        GLint lightAmbientLoc = glGetUniformLocation(lightingShader.Program, "light.ambient");
        GLint lightDiffuseLoc = glGetUniformLocation(lightingShader.Program, "light.diffuse");
        GLint lightSpecularLoc = glGetUniformLocation(lightingShader.Program, "light.specular");
        glUniform3f(lightAmbientLoc, ambientColor.x, ambientColor.y, ambientColor.z);
        glUniform3f(lightDiffuseLoc, diffuseColor.x, diffuseColor.y, diffuseColor.z);
        glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);
        
        // material properties
        GLint matAmbientLoc = glGetUniformLocation(lightingShader.Program, "material.ambient");
        GLint matDiffuseLoc = glGetUniformLocation(lightingShader.Program, "material.diffuse");
        GLint matSpecularLoc = glGetUniformLocation(lightingShader.Program, "material.specular");
        GLint matShineLoc = glGetUniformLocation(lightingShader.Program, "material.shininess");
        
        glUniform3f(matAmbientLoc, materials[curMat * 10 + 0], materials[curMat * 10 + 1], materials[curMat * 10 + 2]);
        glUniform3f(matDiffuseLoc, materials[curMat * 10 + 3], materials[curMat * 10 + 4], materials[curMat * 10 + 5]);
        glUniform3f(matSpecularLoc, materials[curMat * 10 + 6], materials[curMat * 10 + 7], materials[curMat * 10 + 8]);
        glUniform1f(matShineLoc, materials[curMat * 10 + 9]);
        
        
        // Create camera transformations
        glm::mat4 view(1);
        view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
        // Get the uniform locations
        GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
        GLint viewLoc  = glGetUniformLocation(lightingShader.Program,  "view");
        GLint projLoc  = glGetUniformLocation(lightingShader.Program,  "projection");
        // Pass the matrices to the shader
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        
        // Draw the container (using container's vertex attributes)
        glBindVertexArray(containerVAO);
        glm::mat4 model(1);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, num_faces*3);
        glBindVertexArray(0);
        
        // Also draw the lamp object, again binding the appropriate shader
        lampShader.Use();
        // Get location objects for the matrices on the lamp shader (these could be different on a different shader)
        modelLoc = glGetUniformLocation(lampShader.Program, "model");
        viewLoc  = glGetUniformLocation(lampShader.Program, "view");
        projLoc  = glGetUniformLocation(lampShader.Program, "projection");
        // Set matrices
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        model = glm::mat4(1);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        // Draw the light object (using light's vertex attributes)
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        
        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
    
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
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void do_movement()
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
    if (keys[GLFW_KEY_R])
        camera.ProcessKeyboard(UP, deltaTime);
    if (keys[GLFW_KEY_F])
        camera.ProcessKeyboard(DOWN, deltaTime);

    if (keys[GLFW_KEY_P])
        curMat = 0;
    if (keys[GLFW_KEY_O])
        curMat = 1;
    if (keys[GLFW_KEY_I])
        curMat = 2;

    if (keys[GLFW_KEY_L])
        curLight = 0;
    if (keys[GLFW_KEY_K])
        curLight = 1;
    if (keys[GLFW_KEY_J])
        curLight = 2;
    if (keys[GLFW_KEY_H])
        curLight = 3;
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
    GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left
    
    lastX = xpos;
    lastY = ypos;
    
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
