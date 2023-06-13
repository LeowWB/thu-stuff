// Std. Includes
#include <string>
#include <map>
#include <tuple>
#include <cmath>

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
// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

// Other Libs
#include <SOIL.h>

#include "SphereVertices.h"

// Properties
GLuint WIDTH = 800, HEIGHT = 600;

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
GLuint loadTexture(const char* path, GLboolean alpha = false);

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// values not perfectly indicative of real-life values
float rotPhases[] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
float revPhases[] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
float xPos[] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
float zPos[] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
float rotSpeeds[] = {7200.0f, 1000.83f, 300.52f, 1574.0f, 866.0f, 3558.0f, 2684.0f, 1479.0f, 971.0f};
float revSpeeds[] = {1.0f, 8.26f, 3.23f, 1.99f, 1.5f, 1.0f, 0.6f, 0.5f, 0.4f}; 
float radius[] = { 0.0f, 10.0f, 20.0f, 30.0f, 40.0f, 55.0f, 70.0f, 85.0f, 100.0f };
float sizes[] = { 6.5f, 1.5f, 1.7f, 2.0f, 1.7f, 6.0f, 5.0f, 5.5f, 5.5f };
std::string names[] = {"Sun", "Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune"};
float moonRotPhase = 0.0f;
float moonRevPhase = 0.0f;
float moonXPos = 0.0f;
float moonZPos = 0.0f;
float moonRad = 3.0f;

GLfloat rotMod = 1.0f;
GLfloat revMod = 1.0f;

bool showUsageText = true;
bool showPlanetNames = true;

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    GLuint Advance;    // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;
GLuint textVAO, textVBO;
void RenderText(Shader& shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

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

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "A2", nullptr, nullptr); // Windowed
    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);

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
    Shader textShader("text.vert.glsl", "text.frag.glsl");

    GLuint sphereVAO, sphereVBO;
    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphereVertices), &sphereVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glBindVertexArray(0);


    // Load textures
    GLuint sunTex = loadTexture("sun.jpg");
    GLuint merTex = loadTexture("mercury.jpg");
    GLuint venTex = loadTexture("venus.jpg");
    GLuint earTex = loadTexture("earth.jpg");
    GLuint marTex = loadTexture("mars.jpg");
    GLuint jupTex = loadTexture("jupiter.jpg");
    GLuint satTex = loadTexture("saturn.jpg");
    GLuint uraTex = loadTexture("uranus.jpg");
    GLuint nepTex = loadTexture("neptune.jpg");
    GLuint texs[] = { sunTex, merTex, venTex, earTex, marTex, jupTex, satTex, uraTex, nepTex };
    GLuint mooTex = loadTexture("moon.jpg");
    
    // FreeType
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

    // Load font as face
    FT_Face face;
    if (FT_New_Face(ft, "arial.ttf", 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

    // Set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 48);

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Load first 128 characters of ASCII set
    for (GLubyte c = 0; c < 128; c++)
    {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            GLuint(face->glyph->advance.x)
        };
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    // Destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // Configure VAO/VBO for texture quads
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Camera
    glm::mat4 model(1);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, -1000.0f, 1000.0f);

    // Pass projection to shaders
    shader.Use();
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    textShader.Use();
    glUniformMatrix4fv(glGetUniformLocation(textShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (keys['Q']) {
            rotMod += deltaTime;
        }
        
        if (keys['W']) {
            rotMod -= deltaTime;
        }

        if (keys['A']) {
            revMod += deltaTime * 10.0f;
        }

        if (keys['S']) {
            revMod -= deltaTime * 10.0f;
        }

        if (keys['Z']) {
            for (int i = 0; i < 9; i++) {
                revPhases[i] += deltaTime * 2.0f;
            }
        }

        if (keys['X']) {
            for (int i = 0; i < 9; i++) {
                revPhases[i] -= deltaTime * 2.0f;
            }
        }

        // Check and call events
        glfwPollEvents();

        // Clear the colorbuffer
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // planets
        for (int i = 0; i < 9; i++) {
            rotPhases[i] += rotSpeeds[i] * deltaTime * 0.001 * rotMod;
            revPhases[i] -= revSpeeds[i] * deltaTime * 1.0f * revMod;
            xPos[i] = radius[i] * cos(revPhases[i]);
            zPos[i] = radius[i] * sin(revPhases[i]);

            shader.Use();
            glBindVertexArray(sphereVAO);
            glBindTexture(GL_TEXTURE_2D, texs[i]);
            model = glm::mat4(1);
            model = glm::translate(model, glm::vec3(xPos[i], 0.0f, zPos[i]));
            model = glm::rotate(model, rotPhases[i], glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(sizes[i], sizes[i], sizes[i]));
            glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 8 * 3 * 4 * 4 * 4 * 4);

            if (showPlanetNames) {
                textShader.Use();
                RenderText(textShader, names[i], xPos[i] - 5.0f, float(i) * 6.0f + 20.0f, 0.1f, glm::vec3(1.0f, 1.0f, 1.0f));
            }
        }

        // moon
        moonRotPhase += 1000.0 * deltaTime * 0.001 * rotMod;
        moonRevPhase -= 10.0 * deltaTime * 1.0f * revMod;
        moonXPos = moonRad * cos(moonRevPhase) + xPos[3];
        moonZPos = moonRad * sin(moonRevPhase) + zPos[3];

        shader.Use();
        glBindVertexArray(sphereVAO);
        glBindTexture(GL_TEXTURE_2D, mooTex);
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(moonXPos, 0.0f, moonZPos));
        model = glm::rotate(model, moonRotPhase, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 8 * 3 * 4 * 4 * 4 * 4);

        if (showPlanetNames) {
            textShader.Use();
            RenderText(textShader, "Moon", moonXPos - 5.0f, 74.0f, 0.1f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        // text
        if (showUsageText) {
            textShader.Use();
            RenderText(textShader, "Q/W - Change rotation speed (current: " + std::to_string(rotMod) + ")", -100.0f, -80.0f, 0.1f, glm::vec3(1.0f, 1.0f, 1.0f));
            RenderText(textShader, "A/S - Change revolution speed (current: " + std::to_string(revMod) + ")", -100.0f, -85.0f, 0.1f, glm::vec3(1.0f, 1.0f, 1.0f));
            RenderText(textShader, "Z/X - rotate around Sun", -100.0f, -90.0f, 0.1f, glm::vec3(1.0f, 1.0f, 1.0f));
            RenderText(textShader, "O - toggle planet names", -100.0f, -95.0f, 0.1f, glm::vec3(1.0f, 1.0f, 1.0f));
            RenderText(textShader, "P - toggle usage text", -100.0f, -100.0f, 0.1f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        // Swap the buffers
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

void RenderText(Shader& shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
    // Activate corresponding render state
    shader.Use();
    glUniform3f(glGetUniformLocation(shader.Program, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVAO);

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat zPos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;
        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos,     zPos + h,   0.0, 1.0 },
            { xpos,     zPos,       0.0, 0.0 },
            { xpos + w, zPos,       1.0, 0.0 },

            { xpos,     zPos + h,   0.0, 1.0 },
            { xpos + w, zPos,       1.0, 0.0 },
            { xpos + w, zPos + h,   1.0, 1.0 }
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

// This function loads a texture from file. Note: texture loading functions like these are usually
// managed by a 'Resource Manager' that manages all resources (like textures, models, audio).
// For learning purposes we'll just define it as a utility function.
GLuint loadTexture(const char* path, GLboolean alpha)
{
    //Generate texture ID and load texture data
    GLuint textureID;
    glGenTextures(1, &textureID);
    int width, height;
    unsigned char* image = SOIL_load_image(path, &width, &height, 0, alpha ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);
    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, alpha ? GL_RGBA : GL_RGB, width, height, 0, alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT);    // Use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes value from next repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image);
    return textureID;
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

    if (key == 'P' && action == GLFW_RELEASE) {
        showUsageText = !showUsageText;
    }
    
    if (key == 'O' && action == GLFW_RELEASE) {
        showPlanetNames = !showPlanetNames;
    }
}
