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
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

GLfloat rotAmts[] = { 0.0f, 0.0f, 0.0f };
glm::vec3 transAmts = { 0.0f, 0.0f, 0.0f };
float rgbAmts[] = { 1.0f, 1.0f, 1.0f };

int disp_mode = 0;
bool keys[1024];
GLfloat deltaTime = 0.0f;    // Time between current frame and last frame
GLfloat lastFrame = 0.0f;      // Time of last frame
bool altIter = false;

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// The MAIN function, from here we start the application and run the game loop
int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for mac
	#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
	#endif
	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
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
	Shader ourShader("main.vert.glsl", "main.frag.glsl");
	Shader uniShader("uni.vert.glsl", "main.frag.glsl");

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
				
				GLfloat col[3];
				for (int i = 0; i < 3; i++) {
					col[i] = std::rand() / ((float)RAND_MAX);
				}
				
				for (int i = 0; i < 3; i++) {
					int next_v_def_ind = std::stoi(ps[i]) - 1;
					for (int j = 0; j < 3; j++) {
						vertices[next_vertex_ind++] = vertex_defs[3 * next_v_def_ind + j];
					}
					for (int j = 0; j < 3; j++) {
						vertices[next_vertex_ind++] = col[j];
					}
				}
			}
		}
	}
	else {
		std::cout << "error when opening file, file not open";
	}

	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * num_faces * 18, vertices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// TexCoord attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO
    

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		altIter = !altIter;

		if (keys['Q']) {
			rotAmts[0] += deltaTime;
		}

		if (keys['W']) {
			rotAmts[1] += deltaTime;
		}

		if (keys['E']) {
			rotAmts[2] += deltaTime;
		}

		if (keys['R']) {
			rotAmts[0] -= deltaTime;
		}

		if (keys['T']) {
			rotAmts[1] -= deltaTime;
		}

		if (keys['Y']) {
			rotAmts[2] -= deltaTime;
		}

		if (keys['A']) {
			transAmts[0] += deltaTime;
		}

		if (keys['S']) {
			transAmts[1] += deltaTime;
		}

		if (keys['D']) {
			transAmts[2] += deltaTime;
		}

		if (keys['F']) {
			transAmts[0] -= deltaTime;
		}

		if (keys['G']) {
			transAmts[1] -= deltaTime;
		}

		if (keys['H']) {
			transAmts[2] -= deltaTime;
		}

		if (keys['Z']) {
			rgbAmts[0] += deltaTime;
			if (rgbAmts[0] > 1.0f) {
				rgbAmts[0] -= 1.0f;
			}
		}

		if (keys['X']) {
			rgbAmts[1] += deltaTime;
			if (rgbAmts[1] > 1.0f) {
				rgbAmts[1] -= 1.0f;
			}
		}

		if (keys['C']) {
			rgbAmts[2] += deltaTime;
			if (rgbAmts[2] > 1.0f) {
				rgbAmts[2] -= 1.0f;
			}
		}

		if (keys['V']) {
			rgbAmts[0] -= deltaTime;
			if (rgbAmts[0] < 0.0f) {
				rgbAmts[0] += 1.0f;
			}
		}

		if (keys['B']) {
			rgbAmts[1] -= deltaTime;
			if (rgbAmts[1] < 0.0f) {
				rgbAmts[1] += 1.0f;
			}
		}

		if (keys['N']) {
			rgbAmts[2] -= deltaTime;
			if (rgbAmts[2] < 0.0f) {
				rgbAmts[2] += 1.0f;
			}
		}

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create transformations
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		model = glm::rotate(model, (GLfloat)rotAmts[0] * glm::radians(50.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, (GLfloat)rotAmts[1] * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, (GLfloat)rotAmts[2] * glm::radians(50.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, transAmts);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		// Note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
		projection = glm::perspective(glm::radians(45.0f), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		// Get their uniform location
		GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
		GLint projLoc = glGetUniformLocation(ourShader.Program, "projection");
		GLint uModelLoc = glGetUniformLocation(uniShader.Program, "model");
		GLint uViewLoc = glGetUniformLocation(uniShader.Program, "view");
		GLint uProjLoc = glGetUniformLocation(uniShader.Program, "projection");
		GLint colorLoc = glGetUniformLocation(uniShader.Program, "realColor");
		// Pass them to the shaders
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uModelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uViewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(uProjLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3f(colorLoc, rgbAmts[0], rgbAmts[1], rgbAmts[2]);
		// Draw container
		glBindVertexArray(VAO);

		if (disp_mode == 0) {
			ourShader.Use();
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawArrays(GL_TRIANGLES, 0, num_faces * 3);
		}
		else if (disp_mode == 1) {
			uniShader.Use();
			glDrawArrays(GL_POINTS, 0, num_faces * 3);
		}
		else if (disp_mode == 2) {
			uniShader.Use();
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawArrays(GL_TRIANGLES, 0, num_faces * 3);
		}
		else if (disp_mode == 3) {
			ourShader.Use();
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawArrays(GL_TRIANGLES, 0, num_faces * 3);
			uniShader.Use();
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawArrays(GL_TRIANGLES, 0, num_faces * 3);
			if (altIter) {
				ourShader.Use();
			}
			else {
				uniShader.Use();
			}
		}
		
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

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE) {
			keys[key] = false;
			
			if (key == 'M') {
				disp_mode = (disp_mode + 1) % 4;
				std::cout << "changing mode to " << disp_mode << std::endl;
			}
		}
	}
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
