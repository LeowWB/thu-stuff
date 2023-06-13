#include <iostream>

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
#include "objreader.h"

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// buffer binding & drawing functions
GLsizei bindFaces(GLuint VAO, GLuint VBO, const Obj& obj);
void drawFaces(Shader& shader, GLuint VAO, int num);

GLsizei bindVertices(GLuint VAO, GLuint VBO, const Obj& obj);
void drawVertices(Shader& shader, GLuint VAO, int num);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// hyper-parameters
glm::vec3 translation = glm::vec3(0.0f, 0.0f, -2.0f);
glm::vec3 rotationAxis = glm::vec3(-1.0f, 1.0f, -1.0f);
glm::vec3 color = glm::vec3(0.1f, 0.95f, 0.1f); 
GLfloat rotationDegrees = 0.0f;
GLfloat displayMode = 0.0f;

constexpr float ROTATE_SPEED = glm::radians(5.0f);
constexpr float TRANSLATE_SPEED = 0.15f;

constexpr glm::vec3 GLM_UP(0.0f, 1.0f, 0.0f);
constexpr glm::vec3 GLM_RIGHT(0.0f, 0.0f, 1.0f);
constexpr glm::vec3 GLM_DOWN = -GLM_UP;
constexpr glm::vec3 GLM_LEFT = -GLM_RIGHT;

// edge color: init to be light green
GLfloat edgeColor[3] = { 0.1f, 0.95f, 0.1f };

// pointers to model / view / projection matrices
std::unique_ptr<glm::mat4> init_model = nullptr;
std::unique_ptr<glm::mat4> model = nullptr;
std::unique_ptr<glm::mat4> view = nullptr;
std::unique_ptr<glm::mat4> projection = nullptr;


// object file path
const char* const OBJ_FILE = "eight.uniform.obj";

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
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
	#endif
	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Tutorial...Loading model", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);

	// Initialize GLAD to setup the OpenGL Function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize glad" << std::endl;
		return -1;
	};

	// Setup OpenGL options
	glEnable(GL_DEPTH_TEST);

	// Build and compile our shader program
	Shader pointShader("point.vert.glsl", "point.frag.glsl");
	Shader faceShader("face.vert.glsl", "face.frag.glsl");

	// load model
	std::ifstream objfile;
	Obj my_obj;

	// ensures ifstream objects can throw exceptions:
	objfile.exceptions(std::ifstream::badbit);
	try {
		objfile.open(OBJ_FILE, std::ios::in);
		objfile >> my_obj;
		objfile.close();
	}
	catch (const std::ifstream::failure& e) {
		std::cerr << "Load obj file '" << OBJ_FILE << "' error: " << e.what() << std::endl;
		glfwTerminate();
		return -4;;
	}

	// alloc VAOs & VBOs for drawing faces and vertices
	GLuint VAO[2];
	GLuint VBO[2];
	// record # of vertices to draw for each case
	GLsizei nVert[2];
	glGenVertexArrays(2, VAO);
	glGenBuffers(2, VBO);

	// bind VAO and VBO for drawing vertices
	nVert[0] = bindVertices(VAO[0], VBO[0], my_obj);
	// bind VAO and VBO for drawing faces
	nVert[1] = bindFaces(VAO[1], VBO[1], my_obj);

	// Create transformations
	init_model = std::make_unique<glm::mat4>(glm::rotate(
		glm::rotate(glm::mat4(1.0f), glm::radians(50.0f), GLM_UP),
		glm::radians(70.0f), GLM_RIGHT));
	//init_model = std::make_unique<glm::mat4>(1.0f);
	model = std::make_unique<glm::mat4>(*init_model);
	view = std::make_unique<glm::mat4>(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f)));
	projection = std::make_unique<glm::mat4>(
		glm::perspective(glm::radians(45.0f), (GLfloat)800.0 / (GLfloat)600.0, 0.1f, 100.0f));
	
	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// display mode & activate shader
		if (displayMode == 0) {
			drawVertices(pointShader, VAO[0], nVert[0]);
		}
			
		else if (displayMode == 1) {
			drawFaces(faceShader, VAO[1], nVert[1]);
		}

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, VBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	else if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{
		if (displayMode == 1.0f)
		{
			displayMode = 0.0f;
		}
		else
		{
			displayMode = 1.0f;
		}
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

GLsizei bindFaces(GLuint VAO, GLuint VBO, const Obj& obj)
{
	// bind VAO
	glBindVertexArray(VAO);

	// bind VBO, buffer data to it
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	std::vector<GLfloat> data;
	for (const auto& face : obj.faces) {
		for (int i = 0; i < 3; i++) {
			int vid = face[i] - 1;
			data.emplace_back(obj.vertices[vid].x);
			data.emplace_back(obj.vertices[vid].y);
			data.emplace_back(obj.vertices[vid].z);
		}
	}

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * data.size(), &data.front(), GL_STATIC_DRAW);

	// set vertex attribute pointers
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0); 

	// unbind VBO & VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return GLsizei(data.size() / 3);
}

void drawFaces(Shader& shader, GLuint VAO, int num)
{
	shader.Use();

	// get uniform locations
	GLint modelLoc = glGetUniformLocation(shader.Program, "model");
	GLint viewLoc = glGetUniformLocation(shader.Program, "view");
	GLint projLoc = glGetUniformLocation(shader.Program, "projection");

	// pass uniform values to shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(*model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(*view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(*projection));

	// use the same color for all points
	GLint colorLoc = glGetUniformLocation(shader.Program, "ourColor");
	glUniform3fv(colorLoc, 1, glm::value_ptr(color));

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, num);
	glBindVertexArray(0);
}

GLsizei bindVertices(GLuint VAO, GLuint VBO, const Obj& obj)
{
	// bind VAO
	glBindVertexArray(VAO);

	// bind VBO, buffer data to it
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * obj.numVertices(), &obj.vertices.front(), GL_STATIC_DRAW);

	// set vertex attribute pointers
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// unbind VBO & VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return GLsizei(obj.numVertices());
}

void drawVertices(Shader& shader, GLuint VAO, int num)
{
	shader.Use();

	// get uniform locations
	GLint modelLoc = glGetUniformLocation(shader.Program, "model");
	GLint viewLoc = glGetUniformLocation(shader.Program, "view");
	GLint projLoc = glGetUniformLocation(shader.Program, "projection");

	// pass uniform values to shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(*model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(*view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(*projection));

	// use the same color for all points
	GLint colorLoc = glGetUniformLocation(shader.Program, "ourColor");
	glUniform3fv(colorLoc, 1, glm::value_ptr(color));

	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, num);
	glBindVertexArray(0);
}