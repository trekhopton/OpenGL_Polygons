#define GLFW_INCLUDE_NONE

#include <math.h>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
// GLEW loads OpenGL extensions. Required for all OpenGL programs.
#include <GL/glew.h>
// GLFW is a cross-platform windowing API, see glfw.org
#include <GLFW/glfw3.h>
// GLM is a matrix library for OpenGL and GLSL applications
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// Utility code to load and compile GLSL shader programs
#include "shader.hpp"

#define VALS_PER_VERT 3

// Handle to our VAO generated in setShaderData method
unsigned int vertexVaoHandles[4];

// Handle to our shader program
unsigned int programID;

float rotation = 0.0f;

int numberOfPolys = 4;
int numberOfVerticesTotal = 0;
std::vector<int> numberOfVertices;
std::vector<int> sidedness;
bool wireFrameOn = false;

std::vector<float> createPoly(int points, float radius){

	float angles[points];

	for(int i = 0; i < points; i++){
		angles[i] = M_PI * 2 * i / points;
	}

	std::vector<float> vertices;

	for(int i = 0; i < points - 2; i++){
		vertices.push_back(0 + radius * cos(angles[0]));
		vertices.push_back(0 + radius * sin(angles[0]));
		vertices.push_back(0);

		vertices.push_back(0 + radius * cos(angles[i+1]));
		vertices.push_back(0 + radius * sin(angles[i+1]));
		vertices.push_back(0);

		vertices.push_back(0 + radius * cos(angles[i+2]));
		vertices.push_back(0 + radius * sin(angles[i+2]));
		vertices.push_back(0);

	}

	return vertices;
}

int setVertexData(){

	std::vector< std::vector<float> > polys;
	std::vector<unsigned int> buffers;

	for(int i = 0; i < numberOfPolys; i++){
		polys.push_back(createPoly(sidedness[i], 0.25));
		buffers.push_back(0);
	}

	for(int i = 0; i < numberOfPolys; i++){
		// Generate storage on the GPU for our triangle and make it current.
		// A VAO is a set of data buffers on the GPU
	    glGenVertexArrays(1, &vertexVaoHandles[i]);
		glBindVertexArray(vertexVaoHandles[i]);

		// Generate new buffers in our VAO
		// A single data buffer store for per-vertex attributes (e.g. position)
		glGenBuffers(1, &buffers[i]);

		// Allocate GPU memory for our vertices and copy them over
		glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * polys[i].size(), &polys[i][0], GL_STATIC_DRAW);

		// Now we tell OpenGL how to interpret the data we just gave it
		// Tell OpenGL what shader variable it corresponds to
	    // Tell OpenGL how it's formatted (floating point, 3 values per vertex)
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);
	}

	// An argument of zero un-binds all VAO's and stops us
    // from accidentally changing the VAO state
	glBindVertexArray(0);

	// The same is true for buffers, so we un-bind it too
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return 0;	// return success
}


/**
 * Renders a frame of the state and shaders we have set up to the window
 * Executed each time a frame is to be drawn.
 */
void render(double timePassed)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Specify the shader program we want to use
    glUseProgram(programID);

	//update the shaders time
	// int timeHandle = glGetUniformLocation(programID, "u_time");
	// if (timeHandle == -1)
	// 	exit(1);
	// glUniform1f(timeHandle, float(timePassed));

	//define the two colours to use
	glm::vec4 colour1 = glm::vec4(0.3, 0.8, 0.5, 1.0);
	glm::vec4 colour2 = glm::vec4(0.1, 0.4, 0.5, 1.0);
	//get reference to the colour in the shader
	int colourHandle = glGetUniformLocation(programID, "colour");

	// Rotate at 0.8 radians per second
	rotation = float(0.6 * timePassed);
	// Get a reference to the transform in the shader, so we can update it
	int mtxHandle = glGetUniformLocation(programID, "transform");
	if (mtxHandle == -1)
		exit(1);
	// Our 4x4 transformation matrix. Initially identity matrix.
	std::vector<glm::mat4> transMatrices;
	for(int i = 0; i < numberOfPolys; i++){
		glm::mat4 mat;
		transMatrices.push_back(mat);
	}

	// --- TOP LEFT ---

	// Make the VAO with our vertex data buffer current
	glBindVertexArray(vertexVaoHandles[0]);
	// set the colour of the polygon
	glUniform4fv(colourHandle, 1, glm::value_ptr(colour1));
    // Update the matrix in the vertex shader
    // We apply a combined rotation and translation
    // The rotation increases each frame, but translation is fixed.
	transMatrices[0] = glm::translate(transMatrices[0], glm::vec3(-0.5f, 0.5f, 0.0f));
    transMatrices[0] = glm::rotate(transMatrices[0], rotation, glm::vec3(0.0f, 0.0f, 1.0f));
	transMatrices[0] = glm::scale(transMatrices[0], glm::vec3(0.5f, 0.5f, 0.0f));

	glUniformMatrix4fv( mtxHandle, 1, false, glm::value_ptr(transMatrices[0]) );
	// Send command to GPU to draw the data in the current VAO as triangles
    glDrawArrays(GL_TRIANGLES, 0, numberOfVertices[0]);

	// --- TOP RIGHT ---

	glBindVertexArray(vertexVaoHandles[1]);
    transMatrices[1] = glm::translate(transMatrices[1], glm::vec3(0.5f, 0.5f, 0.0f));
    transMatrices[1] = glm::rotate(transMatrices[1], rotation, glm::vec3(0.0f, 1.0f, 0.0f));
    transMatrices[1] = glm::rotate(transMatrices[1], float(M_PI), glm::vec3(1.0f, 0.0f, 0.0f));
    glUniformMatrix4fv(mtxHandle, 1, false, glm::value_ptr(transMatrices[1]));
	glDrawArrays(GL_TRIANGLES, 0, numberOfVertices[1]);

	// --- BOTTOM LEFT ---

	glBindVertexArray(vertexVaoHandles[2]);
	glUniform4fv(colourHandle, 1, glm::value_ptr(colour2));
    transMatrices[2] = glm::translate(transMatrices[2], glm::vec3(-0.5f, -0.5f, 0.0f));
    transMatrices[2] = glm::rotate(transMatrices[2], rotation, glm::vec3(0.0f, 0.0f, 1.0f));
	transMatrices[2] = glm::scale(transMatrices[2], glm::vec3(0.8f, 0.8f, 0.0f));
    glUniformMatrix4fv(mtxHandle, 1, false, glm::value_ptr(transMatrices[2]));
	glDrawArrays(GL_TRIANGLES, 0, numberOfVertices[2]);

	// --- BOTTOM RIGHT ---

	glBindVertexArray(vertexVaoHandles[3]);
    transMatrices[3] = glm::translate(transMatrices[3], glm::vec3(0.5f, -0.5f, 0.0f));
	transMatrices[3] = glm::rotate(transMatrices[3], rotation, glm::vec3(0.0f, 1.0f, 0.0f));
    transMatrices[3] = glm::rotate(transMatrices[3], float(M_PI), glm::vec3(1.0f, 0.0f, 0.0f));
	transMatrices[3] = glm::scale(transMatrices[3], glm::vec3(0.4f, 0.4f, 0.0f));
    glUniformMatrix4fv( mtxHandle, 1, false, glm::value_ptr(transMatrices[3]) );
	glDrawArrays(GL_TRIANGLES, 0, numberOfVertices[3]);

	glBindVertexArray(0);	// Un-bind the VAO
	glFlush();	// Guarantees previous commands have been completed before continuing
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    // Close the application by pressing Esc
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
	if (key == GLFW_KEY_L && action == GLFW_PRESS) {
		if(!wireFrameOn){
			wireFrameOn = true;
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		} else {
			wireFrameOn = false;
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
    }
}


/**
 * Error callback for GLFW. Simply prints error message to stderr.
 */
static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

/**
 * Program entry. Sets up OpenGL state, GLSL Shaders and GLFW window and function call backs
 * Takes no arguments
 */
int main(int argc, char **argv)
{
	int sides;
	for(int i = 0; i < numberOfPolys; i++){
		if(i < numberOfPolys / 2){
			sides = atoi(argv[1]);
		} else {
			sides = atoi(argv[2]);
		}
		sidedness.push_back(sides);
	}



	for(int i = 0; i < numberOfPolys; i++){
		numberOfVerticesTotal += 3 * sidedness[i] - 2;
		numberOfVertices.push_back(3 * sidedness[i] - 2);
	}

	GLFWwindow* window;
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        exit(1);
    }

    // Specify that we want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create the window and OpenGL context
    window = glfwCreateWindow(800, 800, "It's made of triangles but it's not a triangle!", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

	// Tell GLFW to use our callback function when key pressed.
	glfwSetKeyCallback(window, key_callback);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		exit(1);
	}

	// Sets the (background) colour for each time the frame-buffer
    // (colour buffer) is cleared
	glClearColor(0.4f, 0.2f, 0.2f, 1.0f);

	// Set up the shaders we are to use. 0 indicates error.
	programID = LoadShaders("minimal.vert", "minimal.frag");
	if (programID == 0)
		exit(1);

	// Set the vertex data for the program
	if (setVertexData() != 0)
		exit(1);

	// GLFW records the time since the program started (in seconds)
	double start = glfwGetTime();
	double now;

    // The event loop, runs until the window is closed.
    // Each iteration redraws the window contents and checks for new events.
    // Windows are double buffered, so need to swap buffers.

    while (!glfwWindowShouldClose(window)){

		now = glfwGetTime();

        render(now - start);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(0);

	return 0;
}
