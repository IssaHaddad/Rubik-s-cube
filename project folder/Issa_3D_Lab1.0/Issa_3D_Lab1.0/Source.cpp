// Multiple lights
//	see update function for keys


#pragma comment(linker, "/NODEFAULTLIB:MSVCRT")

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#include <GLM/gtx/transform.hpp>
#include <GLI/gli.hpp>

void errorCallbackGLFW(int error, const char* description);
void hintsGLFW();
void endProgram();
void render(GLfloat currentTime);
void update(GLfloat currentTime);
void setupRender();
void startup();
void onResizeCallback(GLFWwindow* window, int w, int h);
void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void onMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void onMouseMoveCallback(GLFWwindow* window, double x, double y);
void onMouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset);
void debugGL();
static void APIENTRY openGLDebugCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const GLvoid* userParam);
string readShader(string name);
void checkErrorShader(GLuint shader);
void readObj(string name, struct modelObject *obj);
void readTexture(string name, GLuint texture);


// VARIABLES

float modelAngle2[] = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f };
float modelAngle3[] = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f };
float modelAngle4[] = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f };

double timer = 0;
bool rotate1[] = { false,false,false,false,false,false,false,false,false };

GLFWwindow*		window;
int				windowWidth = 640;
int				windowHeight = 480;
//int				windowHeight = 480;
bool			running = true;
glm::mat4		proj_matrix;
glm::vec3		modelAngle = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3		modelDisp = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3		cameraPosition = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3		cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float           aspect = (float)windowWidth / (float)windowHeight;
float			fovy = 45.0f;
bool			keyStatus[1024];
GLfloat			deltaTime = 0.0f;
GLfloat			lastTime = 0.0f;

// FPS camera variables
GLfloat			yaw = -90.0f;	// init pointing to inside
GLfloat			pitch = 0.0f;	// start centered
GLfloat			lastX = (GLfloat)windowWidth / 2.0f;	// start middle screen
GLfloat			lastY = (GLfloat)windowHeight / 2.0f;	// start middle screen
bool			firstMouse = true;

// OBJ Variables
struct modelObject {
	std::vector < glm::vec3 > out_vertices;
	std::vector < glm::vec2 > out_uvs;
	std::vector < glm::vec3 > out_normals;
	GLuint*		texture;
	GLuint      program;
	GLuint      vao;
	GLuint      buffer[2];
	GLint       mv_location;
	GLint       proj_location;
	GLint		tex_location;

	// extra variables for this example
	GLuint		matColor_location;
	GLuint		lightColor_location;

} objectModel, lightModel;

glm::vec3		*modelPositions;
glm::vec3		*modelRotations;

// Light
bool			movingLight = true;
int				movingLightNumber = 0;
glm::vec3		*lightDisp;
//glm::vec3		lightDisp = glm::vec3(-1.0f, -1.0f, 0.0f);
glm::vec3		ia = glm::vec3(1.0f, 1.0f, 1.0f);
GLfloat			ka = 5.0;
//glm::vec3		id = glm::vec3(0.93f, 0.75f, 0.32f);
glm::vec3*		id;
GLfloat			kd = 1.0;
glm::vec3		is = glm::vec3(1.0f, 1.0f, 1.0f);
GLfloat			ks = 1.0;


// framebuffer
GLuint			framebuffer;
GLuint			framebufferTexture;
GLuint			depthbuffer;
GLuint			displayVao;
GLuint			displayBuffer[2];
std::vector < glm::vec2 > displayVertices;
std::vector < glm::vec2 > displayUvs;
GLuint			displayProgram;


int main()
{
	if (!glfwInit()) {							// Checking for GLFW
		cout << "Could not initialise GLFW...";
		return 0;
	}

	glfwSetErrorCallback(errorCallbackGLFW);	// Setup a function to catch and display all GLFW errors.

	hintsGLFW();								// Setup glfw with various hints.		

												// Start a window using GLFW
	string title = "My OpenGL Application";

	// Fullscreen
	//const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	//windowWidth = mode->width; windowHeight = mode->height;
	//window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), glfwGetPrimaryMonitor(), NULL);

	// Window
	window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), NULL, NULL);
	if (!window) {								// Window or OpenGL context creation failed
		cout << "Could not initialise GLFW...";
		endProgram();
		return 0;
	}

	glfwMakeContextCurrent(window);				// making the OpenGL context current

												// Start GLEW (note: always initialise GLEW after creating your window context.)
	glewExperimental = GL_TRUE;					// hack: catching them all - forcing newest debug callback (glDebugMessageCallback)
	GLenum errGLEW = glewInit();
	if (GLEW_OK != errGLEW) {					// Problems starting GLEW?
		cout << "Could not initialise GLEW...";
		endProgram();
		return 0;
	}

	debugGL();									// Setup callback to catch openGL errors.	

												// Setup all the message loop callbacks.
	glfwSetWindowSizeCallback(window, onResizeCallback);		// Set callback for resize
	glfwSetKeyCallback(window, onKeyCallback);					// Set Callback for keys
	glfwSetMouseButtonCallback(window, onMouseButtonCallback);	// Set callback for mouse click
	glfwSetCursorPosCallback(window, onMouseMoveCallback);		// Set callback for mouse move
	glfwSetScrollCallback(window, onMouseWheelCallback);		// Set callback for mouse wheel.
																//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);	// Set mouse cursor. Fullscreen
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	// Set mouse cursor FPS.

	setupRender();								// setup some render variables.
	startup();									// Setup all necessary information for startup (aka. load texture, shaders, models, etc).

	do {										// run until the window is closed
		GLfloat currentTime = (GLfloat)glfwGetTime();		// retrieve timelapse
		deltaTime = currentTime - lastTime;		// Calculate delta time
		lastTime = currentTime;					// Save for next frame calculations.
		glfwPollEvents();						// poll callbacks
		update(currentTime);					// update (physics, animation, structures, etc)
		render(currentTime);					// call render function.

		glfwSwapBuffers(window);				// swap buffers (avoid flickering and tearing)

		running &= (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE);	// exit if escape key pressed
		running &= (glfwWindowShouldClose(window) != GL_TRUE);
	} while (running);

	endProgram();			// Close and clean everything up...

	cout << "\nPress any key to continue...\n";
	cin.ignore(); cin.get(); // delay closing console to read debugging errors.

	return 0;
}

void errorCallbackGLFW(int error, const char* description) {
	cout << "Error GLFW: " << description << "\n";
}

void hintsGLFW() {
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);			// Create context in debug mode - for debug message callback
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
}

void endProgram() {
	glfwMakeContextCurrent(window);		// destroys window handler
	glfwTerminate();	// destroys all windows and releases resources.

						// tidy heap memory
	delete[] objectModel.texture;
	delete[] lightModel.texture;
	delete[] modelPositions;
	delete[] modelRotations;
	delete[] lightDisp;
	delete[] id;
}

void setupRender() {
	glfwSwapInterval(1);	// Ony render when synced (V SYNC)

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 0);
	glfwWindowHint(GLFW_STEREO, GL_FALSE);
}

void startup() {

	// Load main object model and shaders

	// --------------Main Model---------------------
	objectModel.program = glCreateProgram();

	string vs_text = readShader("vs_model.glsl"); static const char* vs_source = vs_text.c_str();
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_source, NULL);
	glCompileShader(vs);
	checkErrorShader(vs);
	glAttachShader(objectModel.program, vs);

	string fs_text = readShader("fs_model.glsl"); static const char* fs_source = fs_text.c_str();
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fs_source, NULL);
	glCompileShader(fs);
	checkErrorShader(fs);
	glAttachShader(objectModel.program, fs);

	glLinkProgram(objectModel.program);
	glUseProgram(objectModel.program);

	readObj("RubiksObj.obj", &objectModel);

	glCreateBuffers(3, objectModel.buffer);		// Create a new buffer

												// Store the vertices
	glNamedBufferStorage(objectModel.buffer[0], objectModel.out_vertices.size() * sizeof(glm::vec3), &objectModel.out_vertices[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, objectModel.buffer[0]);	// Bind Buffer

															// Store the texture coordinates
	glNamedBufferStorage(objectModel.buffer[1], objectModel.out_uvs.size() * sizeof(glm::vec2), &objectModel.out_uvs[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, objectModel.buffer[1]);	// Bind Buffer

															// Store the normal Vectors
	glNamedBufferStorage(objectModel.buffer[2], objectModel.out_normals.size() * sizeof(glm::vec3), &objectModel.out_normals[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, objectModel.buffer[3]);	// Bind Buffer

	glCreateVertexArrays(1, &objectModel.vao);		// Create Vertex Array Object

													// Bind vertex position buffer to the vao and format
	glVertexArrayVertexBuffer(objectModel.vao, 0, objectModel.buffer[0], 0, sizeof(GLfloat) * 3);
	glVertexArrayAttribFormat(objectModel.vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(objectModel.vao, 0);	// Enable Vertex Array Attribute

													// Bind texture coordinate buffer to the vao and format
	glVertexArrayVertexBuffer(objectModel.vao, 1, objectModel.buffer[1], 0, sizeof(GLfloat) * 2);
	glVertexArrayAttribFormat(objectModel.vao, 1, 2, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(objectModel.vao, 1);	// Enable Vertex Array Attribute

													// Bind the normals buffer to the vao and format
	glVertexArrayVertexBuffer(objectModel.vao, 2, objectModel.buffer[2], 0, sizeof(GLfloat) * 3);
	glVertexArrayAttribFormat(objectModel.vao, 2, 3, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(objectModel.vao, 2);	// Enable Vertex Array Attribute

	glBindVertexArray(objectModel.vao);				// Bind VertexArrayObject

	objectModel.mv_location = glGetUniformLocation(objectModel.program, "mv_matrix");
	objectModel.proj_location = glGetUniformLocation(objectModel.program, "proj_matrix");
	objectModel.tex_location = glGetUniformLocation(objectModel.program, "tex");
	objectModel.lightColor_location = glGetUniformLocation(objectModel.program, "ia");
	objectModel.lightColor_location = glGetUniformLocation(objectModel.program, "ka");



	//--------------Light Model--------------------------
	lightModel.program = glCreateProgram();

	string vs_textLight = readShader("vs_light.glsl"); static const char* vs_sourceLight = vs_textLight.c_str();
	GLuint vsLight = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vsLight, 1, &vs_sourceLight, NULL);
	glCompileShader(vsLight);
	checkErrorShader(vsLight);
	glAttachShader(lightModel.program, vsLight);

	string fs_textLight = readShader("fs_light.glsl"); static const char* fs_sourceLight = fs_textLight.c_str();
	GLuint fsLight = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fsLight, 1, &fs_sourceLight, NULL);
	glCompileShader(fsLight);
	checkErrorShader(fsLight);
	glAttachShader(lightModel.program, fsLight);

	glLinkProgram(lightModel.program);

	readObj("sphere.obj", &lightModel);

	glCreateBuffers(3, lightModel.buffer);		// Create a new buffer

												// Store the vertices
	glNamedBufferStorage(lightModel.buffer[0], lightModel.out_vertices.size() * sizeof(glm::vec3), &lightModel.out_vertices[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, lightModel.buffer[0]);	// Bind Buffer

															// Store the texture coordinates
	glNamedBufferStorage(lightModel.buffer[1], lightModel.out_uvs.size() * sizeof(glm::vec2), &lightModel.out_uvs[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, lightModel.buffer[1]);	// Bind Buffer

															// Store the normal Vectors
	glNamedBufferStorage(lightModel.buffer[2], lightModel.out_normals.size() * sizeof(glm::vec3), &lightModel.out_normals[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, lightModel.buffer[3]);	// Bind Buffer

	glCreateVertexArrays(1, &lightModel.vao);		// Create Vertex Array Object

													// Bind vertex position buffer to the vao and format
	glVertexArrayVertexBuffer(lightModel.vao, 0, lightModel.buffer[0], 0, sizeof(GLfloat) * 3);
	glVertexArrayAttribFormat(lightModel.vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(lightModel.vao, 0);	// Enable Vertex Array Attribute

													// Bind texture coordinate buffer to the vao and format
	glVertexArrayVertexBuffer(lightModel.vao, 1, lightModel.buffer[1], 0, sizeof(GLfloat) * 2);
	glVertexArrayAttribFormat(lightModel.vao, 1, 2, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(lightModel.vao, 1);	// Enable Vertex Array Attribute

													// Bind the normals buffer to the vao and format
	glVertexArrayVertexBuffer(lightModel.vao, 2, lightModel.buffer[2], 0, sizeof(GLfloat) * 3);
	glVertexArrayAttribFormat(lightModel.vao, 2, 3, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(lightModel.vao, 2);	// Enable Vertex Array Attribute

	glBindVertexArray(lightModel.vao);				// Bind VertexArrayObject

	lightModel.mv_location = glGetUniformLocation(lightModel.program, "mv_matrix");
	lightModel.proj_location = glGetUniformLocation(lightModel.program, "proj_matrix");
	lightModel.tex_location = glGetUniformLocation(lightModel.program, "tex");


	//--------------------------------------------

	modelPositions = new glm::vec3[27];
	modelPositions[0] = glm::vec3(0.0f, 0.0f, 0.0f);
	modelPositions[1] = glm::vec3(0.4f, 0.0f, 0.0f);
	modelPositions[2] = glm::vec3(-0.4f, 0.0f, 0.0f);
	modelPositions[3] = glm::vec3(0.0f, 0.0f, 0.4f);
	modelPositions[4] = glm::vec3(0.4f, 0.0f, 0.4f);
	modelPositions[5] = glm::vec3(-0.4f, 0.0f, 0.4f);
	modelPositions[6] = glm::vec3(0.0f, 0.0f, -0.4f);
	modelPositions[7] = glm::vec3(0.4f, 0.0f, -0.4f);
	modelPositions[8] = glm::vec3(-0.4f, 0.0f, -0.4f);
	modelPositions[9] = glm::vec3(0.0f, 0.4f, 0.0f);
	modelPositions[10] = glm::vec3(0.4f, 0.4f, 0.0f);
	modelPositions[11] = glm::vec3(-0.4f, 0.4f, 0.0f);
	modelPositions[12] = glm::vec3(0.0f, 0.4f, 0.4f);
	modelPositions[13] = glm::vec3(0.4f, 0.4f, 0.4f);
	modelPositions[14] = glm::vec3(-0.4f, 0.4f, 0.4f);
	modelPositions[15] = glm::vec3(0.0f, 0.4f, -0.4f);
	modelPositions[16] = glm::vec3(0.4f, 0.4f, -0.4f);
	modelPositions[17] = glm::vec3(-0.4f, 0.4f, -0.4f);
	modelPositions[18] = glm::vec3(0.0f, -0.4f, 0.0f);
	modelPositions[19] = glm::vec3(0.4f, -0.4f, 0.0f);
	modelPositions[20] = glm::vec3(-0.4f, -0.4f, 0.0f);
	modelPositions[21] = glm::vec3(0.0f, -0.4f, 0.4f);
	modelPositions[22] = glm::vec3(0.4f, -0.4f, 0.4f);
	modelPositions[23] = glm::vec3(-0.4f, -0.4f, 0.4f);
	modelPositions[24] = glm::vec3(0.0f, -0.4f, -0.4f);
	modelPositions[25] = glm::vec3(0.4f, -0.4f, -0.4f);
	modelPositions[26] = glm::vec3(-0.4f, -0.4f, -0.4f);

	modelRotations = new glm::vec3[12];
	modelRotations[0] = glm::vec3(0.0f, 0.0f, 0.0f);
	modelRotations[1] = glm::vec3(20.0f, 10.0f, 0.0f);
	modelRotations[2] = glm::vec3(30.0f, 40.0f, 0.0f);
	modelRotations[3] = glm::vec3(20.0f, 50.0f, 0.0f);
	modelRotations[4] = glm::vec3(40.0f, 50.0f, 0.0f);
	modelRotations[5] = glm::vec3(70.0f, 60.0f, 0.0f);
	modelRotations[6] = glm::vec3(80.0f, 60.0f, 0.0f);
	modelRotations[7] = glm::vec3(10.0f, 20.0f, 0.0f);
	modelRotations[8] = glm::vec3(70.0f, 30.0f, 0.0f);
	modelRotations[9] = glm::vec3(50.0f, 40.0f, 0.0f);
	modelRotations[10] = glm::vec3(60.0f, 80.0f, 0.0f);
	modelRotations[11] = glm::vec3(70.0f, 80.0f, 0.0f);

	lightDisp = new glm::vec3[6];
	lightDisp[0] = glm::vec3(-1.5f, 0.0f, 0.0f);
	lightDisp[1] = glm::vec3(0.0f, 0.0f, 1.5f);
	lightDisp[2] = glm::vec3(1.5f, 0.0f, 0.0f);
	lightDisp[3] = glm::vec3(0.0f, 0.0f, -1.5f);
	lightDisp[4] = glm::vec3(0.0f, 1.5f, 0.0f);
	lightDisp[5] = glm::vec3(0.0f, -1.5f, 0.0f);

	id = new glm::vec3[4];
	id[0] = glm::vec3(1.0f, 0.0f, 0.0f);
	id[1] = glm::vec3(0.0f, 1.0f, 0.0f);
	id[2] = glm::vec3(0.0f, 0.0f, 1.0f);
	id[3] = glm::vec3(1.0f, 1.0f, 0.0f);


	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Calculate proj_matrix for the first time.
	aspect = (float)windowWidth / (float)windowHeight;
	proj_matrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);


	// ----------Start Framebuffer---------------
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// Create a texture for the framebuffer
	glGenTextures(1, &framebufferTexture);

	// Bind this texture so we can modify it.
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);

	//  Start the texture and give it a size but no data- of course if you resize you need to change your texture.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	// filtering needed - future lecture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Depth buffer texture	- Need to attach depth too otherwise depth testing will not be performed.
	glGenRenderbuffers(1, &depthbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);


	// Create a quad to display our framebuffer
	displayVertices.push_back(glm::vec2(-1.0f, 1.0f));
	displayVertices.push_back(glm::vec2(-1.0f, -1.0f));
	displayVertices.push_back(glm::vec2(1.0f, -1.0f));
	displayVertices.push_back(glm::vec2(-1.0f, 1.0f));
	displayVertices.push_back(glm::vec2(1.0f, -1.0f));
	displayVertices.push_back(glm::vec2(1.0f, 1.0f));

	displayUvs.push_back(glm::vec2(0.0f, 1.0f));
	displayUvs.push_back(glm::vec2(0.0f, 0.0f));
	displayUvs.push_back(glm::vec2(1.0f, 0.0f));
	displayUvs.push_back(glm::vec2(0.0f, 1.0f));
	displayUvs.push_back(glm::vec2(1.0f, 0.0f));
	displayUvs.push_back(glm::vec2(1.0f, 1.0f));

	glCreateBuffers(2, displayBuffer);		// Create a new buffer
											// Store the vertices
	glNamedBufferStorage(displayBuffer[0], displayVertices.size() * sizeof(glm::vec2), &displayVertices[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, displayBuffer[0]);	// Bind Buffer
														// Store the texture coordinates
	glNamedBufferStorage(displayBuffer[1], displayUvs.size() * sizeof(glm::vec2), &displayUvs[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, displayBuffer[1]);	// Bind Buffer

	glCreateVertexArrays(1, &displayVao);		// Create Vertex Array Object
												// Bind vertex position buffer to the vao and format
	glVertexArrayVertexBuffer(displayVao, 0, displayBuffer[0], 0, sizeof(GLfloat) * 2);
	glVertexArrayAttribFormat(displayVao, 0, 2, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(displayVao, 0);	// Enable Vertex Array Attribute

												// Bind texture coordinate buffer to the vao and format
	glVertexArrayVertexBuffer(displayVao, 1, displayBuffer[1], 0, sizeof(GLfloat) * 2);
	glVertexArrayAttribFormat(displayVao, 1, 2, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(displayVao, 1);	// Enable Vertex Array Attribute

	glBindVertexArray(displayVao);				// Bind VertexArrayObject

												//load shaders
	displayProgram = glCreateProgram();

	string dvs_text = readShader("vs_display.glsl"); static const char* dvs_source = dvs_text.c_str();
	GLuint dvs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(dvs, 1, &dvs_source, NULL);
	glCompileShader(dvs);
	checkErrorShader(dvs);
	glAttachShader(displayProgram, dvs);

	string dfs_text = readShader("fs_display.glsl"); static const char* dfs_source = dfs_text.c_str();
	GLuint dfs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(dfs, 1, &dfs_source, NULL);
	glCompileShader(dfs);
	checkErrorShader(dfs);
	glAttachShader(displayProgram, dfs);

	glLinkProgram(displayProgram);
	glUseProgram(displayProgram);
}

void update(GLfloat currentTime) {

	// calculate movement
	GLfloat cameraSpeed = 1.0f * deltaTime;
	if (keyStatus[GLFW_KEY_W]) cameraPosition += cameraSpeed * cameraFront;
	if (keyStatus[GLFW_KEY_S]) cameraPosition -= cameraSpeed * cameraFront;
	if (keyStatus[GLFW_KEY_A]) cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keyStatus[GLFW_KEY_D]) cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

	if (keyStatus[GLFW_KEY_L] && (movingLight == false)) {
		cout << "Change mode to moving light...\n";
		movingLight = true;
	}
	if (keyStatus[GLFW_KEY_M] && (movingLight == true)) {
		cout << "Change mode to moving object...\n";
		movingLight = false;
	}

	if (keyStatus[GLFW_KEY_0] && (movingLight == true)) {
		movingLightNumber = 0;  cout << "Moving light 0...\n";
	}
	if (keyStatus[GLFW_KEY_1] && (movingLight == true)) {
		movingLightNumber = 1;  cout << "Moving light 1...\n";
	}
	if (keyStatus[GLFW_KEY_2] && (movingLight == true)) {
		movingLightNumber = 2;  cout << "Moving light 2...\n";
	}
	if (keyStatus[GLFW_KEY_3] && (movingLight == true)) {
		movingLightNumber = 3;  cout << "Moving light 3...\n";
	}

	if (keyStatus[GLFW_KEY_Y])
		rotate1[0] = true;
	if (keyStatus[GLFW_KEY_U])
		rotate1[1] = true;
	if (keyStatus[GLFW_KEY_I])
		rotate1[2] = true;
	if (keyStatus[GLFW_KEY_H])
		rotate1[3] = true;
	if (keyStatus[GLFW_KEY_J])
		rotate1[4] = true;
	if (keyStatus[GLFW_KEY_K])
		rotate1[5] = true;
	if (keyStatus[GLFW_KEY_B])
		rotate1[6] = true;
	if (keyStatus[GLFW_KEY_N])
		rotate1[7] = true;
	if (keyStatus[GLFW_KEY_M])
		rotate1[8] = true;

	if (rotate1[0] == true)
	{
		modelAngle2[1] += 0.1;
		modelAngle2[4] += 0.1;
		modelAngle2[7] += 0.1;
		modelAngle2[10] += 0.1;
		modelAngle2[13] += 0.1;
		modelAngle2[16] += 0.1;
		modelAngle2[19] += 0.1;
		modelAngle2[22] += 0.1;
		modelAngle2[25] += 0.1;
		timer++;
		if (timer >= 63 && rotate1[0] == true)
		{
			rotate1[0] = false;
			timer = 0;
			modelAngle2[1] = 0.0;
			modelAngle2[4] = 0.0;
			modelAngle2[7] = 0.0;
			modelAngle2[10] = 0.0;
			modelAngle2[13] = 0.0;
			modelAngle2[16] = 0.0;
			modelAngle2[19] = 0.0;
			modelAngle2[22] = 0.0;
			modelAngle2[25] = 0.0;
		}
	}

	if (rotate1[1] == true)
	{
		modelAngle2[0] += 0.1;
		modelAngle2[3] += 0.1;
		modelAngle2[6] += 0.1;
		modelAngle2[9] += 0.1;
		modelAngle2[12] += 0.1;
		modelAngle2[15] += 0.1;
		modelAngle2[18] += 0.1;
		modelAngle2[21] += 0.1;
		modelAngle2[24] += 0.1;
		timer++;
		if (timer >= 63 && rotate1[1] == true)
		{
			rotate1[1] = false;
			timer = 0;
			modelAngle2[0] = 0.0;
			modelAngle2[3] = 0.0;
			modelAngle2[6] = 0.0;
			modelAngle2[9] = 0.0;
			modelAngle2[12] = 0.0;
			modelAngle2[15] = 0.0;
			modelAngle2[18] = 0.0;
			modelAngle2[21] = 0.0;
			modelAngle2[24] = 0.0;
		}
	}

	if (rotate1[2] == true)
	{
		modelAngle2[2] += 0.1;
		modelAngle2[5] += 0.1;
		modelAngle2[8] += 0.1;
		modelAngle2[11] += 0.1;
		modelAngle2[14] += 0.1;
		modelAngle2[17] += 0.1;
		modelAngle2[20] += 0.1;
		modelAngle2[23] += 0.1;
		modelAngle2[26] += 0.1;
		timer++;
		if (timer >= 63 && rotate1[2] == true)
		{
			rotate1[2] = false;
			timer = 0;
			modelAngle2[2] = 0.0;
			modelAngle2[5] = 0.0;
			modelAngle2[8] = 0.0;
			modelAngle2[11] = 0.0;
			modelAngle2[14] = 0.0;
			modelAngle2[17] = 0.0;
			modelAngle2[20] = 0.0;
			modelAngle2[23] = 0.0;
			modelAngle2[26] = 0.0;
		}
	}

	if (rotate1[3] == true)
	{
		modelAngle3[18] += 0.1;
		modelAngle3[19] += 0.1;
		modelAngle3[20] += 0.1;
		modelAngle3[21] += 0.1;
		modelAngle3[22] += 0.1;
		modelAngle3[23] += 0.1;
		modelAngle3[24] += 0.1;
		modelAngle3[25] += 0.1;
		modelAngle3[26] += 0.1;
		timer++;
		if (timer >= 63 && rotate1[3] == true)
		{
			rotate1[3] = false;
			timer = 0;
			modelAngle3[18] = 0.0;
			modelAngle3[19] = 0.0;
			modelAngle3[20] = 0.0;
			modelAngle3[21] = 0.0;
			modelAngle3[22] = 0.0;
			modelAngle3[23] = 0.0;
			modelAngle3[24] = 0.0;
			modelAngle3[25] = 0.0;
			modelAngle3[26] = 0.0;
		}
	}




	if (rotate1[4] == true)
	{
		modelAngle3[0] += 0.1;
		modelAngle3[1] += 0.1;
		modelAngle3[2] += 0.1;
		modelAngle3[3] += 0.1;
		modelAngle3[4] += 0.1;
		modelAngle3[5] += 0.1;
		modelAngle3[6] += 0.1;
		modelAngle3[7] += 0.1;
		modelAngle3[8] += 0.1;
		timer++;
		if (timer >= 63 && rotate1[4] == true)
		{
			rotate1[4] = false;
			timer = 0;
			modelAngle3[0] = 0.0;
			modelAngle3[1] = 0.0;
			modelAngle3[2] = 0.0;
			modelAngle3[3] = 0.0;
			modelAngle3[4] = 0.0;
			modelAngle3[5] = 0.0;
			modelAngle3[6] = 0.0;
			modelAngle3[7] = 0.0;
			modelAngle3[8] = 0.0;
		}
	}

	if (rotate1[5] == true)
	{
		modelAngle3[9] += 0.1;
		modelAngle3[10] += 0.1;
		modelAngle3[11] += 0.1;
		modelAngle3[12] += 0.1;
		modelAngle3[13] += 0.1;
		modelAngle3[14] += 0.1;
		modelAngle3[15] += 0.1;
		modelAngle3[16] += 0.1;
		modelAngle3[17] += 0.1;
		timer++;
		if (timer >= 63 && rotate1[5] == true)
		{
			rotate1[5] = false;
			timer = 0;
			modelAngle3[9] = 0.0;
			modelAngle3[10] = 0.0;
			modelAngle3[11] = 0.0;
			modelAngle3[12] = 0.0;
			modelAngle3[13] = 0.0;
			modelAngle3[14] = 0.0;
			modelAngle3[15] = 0.0;
			modelAngle3[16] = 0.0;
			modelAngle3[17] = 0.0;
		}
	}

	if (rotate1[6] == true)
	{
		modelAngle4[3] += 0.1;
		modelAngle4[4] += 0.1;
		modelAngle4[5] += 0.1;
		modelAngle4[12] += 0.1;
		modelAngle4[13] += 0.1;
		modelAngle4[14] += 0.1;
		modelAngle4[21] += 0.1;
		modelAngle4[22] += 0.1;
		modelAngle4[23] += 0.1;
		timer++;
		if (timer >= 63 && rotate1[6] == true)
		{
			rotate1[6] = false;
			timer = 0;
			modelAngle4[3] = 0.0;
			modelAngle4[4] = 0.0;
			modelAngle4[5] = 0.0;
			modelAngle4[12] = 0.0;
			modelAngle4[13] = 0.0;
			modelAngle4[14] = 0.0;
			modelAngle4[21] = 0.0;
			modelAngle4[22] = 0.0;
			modelAngle4[23] = 0.0;
		}
	}

	if (rotate1[7] == true)
	{
		modelAngle4[0] += 0.1;
		modelAngle4[1] += 0.1;
		modelAngle4[2] += 0.1;
		modelAngle4[9] += 0.1;
		modelAngle4[10] += 0.1;
		modelAngle4[11] += 0.1;
		modelAngle4[18] += 0.1;
		modelAngle4[19] += 0.1;
		modelAngle4[20] += 0.1;
		timer++;
		if (timer >= 63 && rotate1[7] == true)
		{
			rotate1[7] = false;
			timer = 0;
			modelAngle4[0] = 0.0;
			modelAngle4[1] = 0.0;
			modelAngle4[2] = 0.0;
			modelAngle4[9] = 0.0;
			modelAngle4[10] = 0.0;
			modelAngle4[11] = 0.0;
			modelAngle4[18] = 0.0;
			modelAngle4[19] = 0.0;
			modelAngle4[20] = 0.0;
		}
	}

	if (rotate1[8] == true)
	{
		modelAngle4[6] += 0.1;
		modelAngle4[7] += 0.1;
		modelAngle4[8] += 0.1;
		modelAngle4[15] += 0.1;
		modelAngle4[16] += 0.1;
		modelAngle4[17] += 0.1;
		modelAngle4[24] += 0.1;
		modelAngle4[25] += 0.1;
		modelAngle4[26] += 0.1;
		timer++;
		if (timer >= 63 && rotate1[8] == true)
		{
			rotate1[8] = false;
			timer = 0;
			modelAngle4[6] = 0.0;
			modelAngle4[7] = 0.0;
			modelAngle4[8] = 0.0;
			modelAngle4[15] = 0.0;
			modelAngle4[16] = 0.0;
			modelAngle4[17] = 0.0;
			modelAngle4[24] = 0.0;
			modelAngle4[25] = 0.0;
			modelAngle4[26] = 0.0;
		}
	}

	if (movingLight == false) {		// moving object rotation and z displacement
		if (keyStatus[GLFW_KEY_LEFT])			modelAngle.y += 0.05f;
		if (keyStatus[GLFW_KEY_RIGHT])			modelAngle.y -= 0.05f;
		if (keyStatus[GLFW_KEY_UP])				modelAngle.x += 0.05f;
		if (keyStatus[GLFW_KEY_DOWN])			modelAngle.x -= 0.05f;
		if (keyStatus[GLFW_KEY_KP_ADD])			modelDisp.z += 0.10f;
		if (keyStatus[GLFW_KEY_KP_SUBTRACT])	modelDisp.z -= 0.10f;
	}
	else {							// moving light displacement x y z
		if (keyStatus[GLFW_KEY_LEFT])			lightDisp[movingLightNumber].x -= 0.05f;
		if (keyStatus[GLFW_KEY_RIGHT])			lightDisp[movingLightNumber].x += 0.05f;
		if (keyStatus[GLFW_KEY_UP])				lightDisp[movingLightNumber].y += 0.05f;
		if (keyStatus[GLFW_KEY_DOWN])			lightDisp[movingLightNumber].y -= 0.05f;
		if (keyStatus[GLFW_KEY_KP_ADD])			lightDisp[movingLightNumber].z += 0.05f;
		if (keyStatus[GLFW_KEY_KP_SUBTRACT])	lightDisp[movingLightNumber].z -= 0.05f;
	}
}

void render(GLfloat currentTime) {

	//==============First Pass====================
	//---Render framebuffer to texture
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);

	glViewport(0, 0, windowWidth, windowHeight);

	// Clear colour buffer
	glm::vec4 backgroundColor = glm::vec4(0.2f, 0.2f, 0.2f, 0.2f); glClearBufferfv(GL_COLOR, 0, &backgroundColor[0]);

	// Clear Deep buffer
	static const GLfloat one = 1.0f; glClearBufferfv(GL_DEPTH, 0, &one);

	// Enable blend
	glEnable(GL_BLEND); 

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// ----------draw main model------------
	glUseProgram(objectModel.program);
	glBindVertexArray(objectModel.vao);
	glUniformMatrix4fv(objectModel.proj_location, 1, GL_FALSE, &proj_matrix[0][0]);

	glUniform4f(glGetUniformLocation(objectModel.program, "viewPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0);
	glUniform4f(glGetUniformLocation(objectModel.program, "ia"), ia.r, ia.g, ia.b, 1.0);
	glUniform1f(glGetUniformLocation(objectModel.program, "ka"), ka);

	glUniform1f(glGetUniformLocation(objectModel.program, "kd"), 1.0f);
	glUniform4f(glGetUniformLocation(objectModel.program, "is"), is.r, is.g, is.b, 1.0);
	glUniform1f(glGetUniformLocation(objectModel.program, "ks"), 1.0f);
	glUniform1f(glGetUniformLocation(objectModel.program, "shininess"), 32.0f);

	glUniform1f(glGetUniformLocation(objectModel.program, "lightConstant"), 1.0f);
	glUniform1f(glGetUniformLocation(objectModel.program, "lightLinear"), 0.7f);
	glUniform1f(glGetUniformLocation(objectModel.program, "lightQuadratic"), 1.8f);

	for (int i = 0; i < 4; i++) {
		glUniform4f(glGetUniformLocation(objectModel.program, ("lights[" + to_string(i) + "].lightPosition").c_str()), lightDisp[i].x, lightDisp[i].y, lightDisp[i].z, 1.0f);
		glUniform4f(glGetUniformLocation(objectModel.program, ("lights[" + to_string(i) + "].id").c_str()), id[i].r, id[i].g, id[i].b, 1.0f);
	}

	//glUniform4f(glGetUniformLocation(objectModel.program, "lightPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0);
	//glUniform4f(glGetUniformLocation(objectModel.program, "lightSpotDirection"), cameraFront.x, cameraFront.y, cameraFront.z, 0.0);
	//glUniform1f(glGetUniformLocation(objectModel.program, "lightSpotCutOff"), glm::cos(glm::radians(12.5f)));
	//glUniform1f(glGetUniformLocation(objectModel.program, "lightSpotOuterCutOff"), glm::cos(glm::radians(15.0f)));

	// Bind textures and samplers - using 0 as I know there is only one texture - need to extend.
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, objectModel.texture[0]);
	glUniform1i(objectModel.tex_location, 0);

	glm::mat4 viewMatrix = glm::lookAt(cameraPosition,					// eye
		cameraPosition + cameraFront,	// centre
		cameraUp);						// up

	for (int i = 0; i < 27; i++) {
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));// modelDisp.z));
		
		modelMatrix = glm::rotate(modelMatrix, modelAngle2[i], glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, modelAngle3[i], glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, modelAngle4[i], glm::vec3(0.0f, 0.0f, 1.0f));
		
		modelMatrix = glm::translate(modelMatrix, modelPositions[i]);
		modelMatrix = glm::rotate(modelMatrix, modelAngle.x + modelRotations[0].x, glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, modelAngle.y + modelRotations[0].y, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.2f, 0.2f));

		glm::mat4 mv_matrix = viewMatrix * modelMatrix;

		glUniformMatrix4fv(glGetUniformLocation(objectModel.program, "model_matrix"), 1, GL_FALSE, &modelMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(objectModel.program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, objectModel.out_vertices.size());
	}

	// ----------draw light------------
	glUseProgram(lightModel.program);
	glBindVertexArray(lightModel.vao);
	glUniformMatrix4fv(lightModel.proj_location, 1, GL_FALSE, &proj_matrix[0][0]);

	// Bind textures and samplers - using 0 as I know there is only one texture - need to extend.
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, lightModel.texture[0]);
	glUniform1i(lightModel.tex_location, 0);

	for (int i = 0; i < 6; i++) {
		glm::mat4 modelMatrixLight = glm::translate(glm::mat4(1.0f), glm::vec3(lightDisp[i].x, lightDisp[i].y, lightDisp[i].z));
		modelMatrixLight = glm::scale(modelMatrixLight, glm::vec3(0.2f, 0.2f, 0.2f));
		glm::mat4 mv_matrixLight = viewMatrix * modelMatrixLight;

		glUniformMatrix4fv(lightModel.mv_location, 1, GL_FALSE, &mv_matrixLight[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, lightModel.out_vertices.size());
	}

	//==============Second Pass===================
	glBindFramebuffer(GL_FRAMEBUFFER, 0);	// Disable rendering framebuffer to texture - aka render normally.

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST); //not needed as we are just displaying a single quad

	glUseProgram(displayProgram);
	glBindVertexArray(displayVao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void onResizeCallback(GLFWwindow* window, int w, int h) {
	windowWidth = w;
	windowHeight = h;

	aspect = (float)w / (float)h;
	proj_matrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);
}

void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) keyStatus[key] = true;
	else if (action == GLFW_RELEASE) keyStatus[key] = false;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void onMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {

}

void onMouseMoveCallback(GLFWwindow* window, double x, double y) {
	int mouseX = static_cast<int>(x);
	int mouseY = static_cast<int>(y);

	if (firstMouse) {
		lastX = (GLfloat)mouseX; lastY = (GLfloat)mouseY; firstMouse = false;
	}

	GLfloat xoffset = mouseX - lastX;
	GLfloat yoffset = lastY - mouseY; // Reversed
	lastX = (GLfloat)mouseX; lastY = (GLfloat)mouseY;

	GLfloat sensitivity = 0.05f;
	xoffset *= sensitivity; yoffset *= sensitivity;

	yaw += xoffset; pitch += yoffset;

	// check for pitch out of bounds otherwise screen gets flipped
	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	cameraFront = glm::normalize(front);

}

static void onMouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset) {
	int yoffsetInt = static_cast<int>(yoffset);

	fovy += yoffsetInt;
	if (fovy >= 1.0f && fovy <= 45.0f) fovy -= (GLfloat)yoffset;
	if (fovy <= 1.0f) fovy = 1.0f;
	if (fovy >= 45.0f) fovy = 45.0f;
	proj_matrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);
}

void debugGL() {
	//Output some debugging information
	cout << "VENDOR: " << (char *)glGetString(GL_VENDOR) << endl;
	cout << "VERSION: " << (char *)glGetString(GL_VERSION) << endl;
	cout << "RENDERER: " << (char *)glGetString(GL_RENDERER) << endl;

	// Enable Opengl Debug
	//glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback((GLDEBUGPROC)openGLDebugCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
}

static void APIENTRY openGLDebugCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const GLvoid* userParam) {

	cout << "---------------------opengl-callback------------" << endl;
	cout << "Message: " << message << endl;
	cout << "type: ";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		cout << "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		cout << "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		cout << "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		cout << "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		cout << "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		cout << "OTHER";
		break;
	}
	cout << " --- ";

	cout << "id: " << id << " --- ";
	cout << "severity: ";
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:
		cout << "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		cout << "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		cout << "HIGH";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		cout << "NOTIFICATION";
	}
	cout << endl;
	cout << "-----------------------------------------" << endl;
}

string readShader(string name) {
	string vs_text;
	std::ifstream vs_file(name);

	string vs_line;
	if (vs_file.is_open()) {

		while (getline(vs_file, vs_line)) {
			vs_text += vs_line;
			vs_text += '\n';
		}
		vs_file.close();
	}
	return vs_text;
}

void  checkErrorShader(GLuint shader) {
	// Get log lenght
	GLint maxLength;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

	// Init a string for it
	std::vector<GLchar> errorLog(maxLength);

	if (maxLength > 0) {
		// Get the log file
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		cout << "--------------Shader compilation error-------------\n";
		cout << errorLog.data();
	}

}

void readObj(string name, struct modelObject *obj) {
	cout << "Loading model " << name << "\n";

	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< string > materials, textures;
	std::vector< glm::vec3 > obj_vertices;
	std::vector< glm::vec2 > obj_uvs;
	std::vector< glm::vec3 > obj_normals;

	std::ifstream dataFile(name);

	string rawData;		// store the raw data.
	int countLines = 0;
	if (dataFile.is_open()) {
		string dataLineRaw;
		while (getline(dataFile, dataLineRaw)) {
			rawData += dataLineRaw;
			rawData += "\n";
			countLines++;
		}
		dataFile.close();
	}

	cout << "Finished reading model file " << name << "\n";

	istringstream rawDataStream(rawData);
	string dataLine;
	int linesDone = 0;
	while (std::getline(rawDataStream, dataLine)) {
		if (dataLine.find("v ") != string::npos) {	// does this line have a vector?
			glm::vec3 vertex;

			int foundStart = dataLine.find(" ");  int foundEnd = dataLine.find(" ", foundStart + 1);
			vertex.x = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
			vertex.y = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
			vertex.z = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			obj_vertices.push_back(vertex);
		}
		else if (dataLine.find("vt ") != string::npos) {	// does this line have a uv coordinates?
			glm::vec2 uv;

			int foundStart = dataLine.find(" ");  int foundEnd = dataLine.find(" ", foundStart + 1);
			uv.x = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
			uv.y = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			obj_uvs.push_back(uv);
		}
		else if (dataLine.find("vn ") != string::npos) { // does this line have a normal coordinates?
			glm::vec3 normal;

			int foundStart = dataLine.find(" ");  int foundEnd = dataLine.find(" ", foundStart + 1);
			normal.x = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
			normal.y = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
			normal.z = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			obj_normals.push_back(normal);
		}
		else if (dataLine.find("f ") != string::npos) { // does this line defines a triangle face?
			string parts[3];

			int foundStart = dataLine.find(" ");  int foundEnd = dataLine.find(" ", foundStart + 1);
			parts[0] = dataLine.substr(foundStart + 1, foundEnd - foundStart - 1);

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
			parts[1] = dataLine.substr(foundStart + 1, foundEnd - foundStart - 1);

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
			parts[2] = dataLine.substr(foundStart + 1, foundEnd - foundStart - 1);

			for (int i = 0; i < 3; i++) {		// for each part

				vertexIndices.push_back(stoul(parts[i].substr(0, parts[i].find("/"))));

				int firstSlash = parts[i].find("/"); int secondSlash = parts[i].find("/", firstSlash + 1);

				if ((firstSlash + 1) != (secondSlash)) {	// there are texture coordinates.
					uvIndices.push_back(stoul(parts[i].substr(firstSlash + 1, secondSlash - firstSlash + 1)));
				}


				normalIndices.push_back(stoul(parts[i].substr(secondSlash + 1)));

			}
		}
		else if (dataLine.find("mtllib ") != string::npos) { // does this object have a material?
			materials.push_back(dataLine.substr(dataLine.find(" ") + 1));
		}

		linesDone++;

		if (linesDone % 50000 == 0) {
			cout << "Parsed " << linesDone << " of " << countLines << " from model...\n";
		}

	}

	// Double check here in which coordinate system you exported your models - and flip or not the vertices...
	
	/*for (unsigned int i = 0; i < vertexIndices.size(); i += 3) {
	(*obj).out_vertices.push_back(obj_vertices[vertexIndices[i+2] - 1]);
	(*obj).out_normals.push_back(obj_normals[normalIndices[i+2] - 1]);
	(*obj).out_uvs.push_back(obj_uvs[uvIndices[i+2] - 1]);

	(*obj).out_vertices.push_back(obj_vertices[vertexIndices[i+1] - 1]);
	(*obj).out_normals.push_back(obj_normals[normalIndices[i+1] - 1]);
	(*obj).out_uvs.push_back(obj_uvs[uvIndices[i + 1] - 1]);

	(*obj).out_vertices.push_back(obj_vertices[vertexIndices[i] - 1]);
	(*obj).out_normals.push_back(obj_normals[normalIndices[i] - 1]);
	(*obj).out_uvs.push_back(obj_uvs[uvIndices[i + 0] - 1]);
	}*/
	

	for (unsigned int i = 0; i < vertexIndices.size(); i++) {
		(*obj).out_vertices.push_back(obj_vertices[vertexIndices[i] - 1]);
		(*obj).out_normals.push_back(obj_normals[normalIndices[i] - 1]);
		(*obj).out_uvs.push_back(obj_uvs[uvIndices[i] - 1]);
	}


	// Load Materials
	for (unsigned int i = 0; i < materials.size(); i++) {

		std::ifstream dataFileMat(materials[i]);

		string rawDataMat;		// store the raw data.
		int countLinesMat = 0;
		if (dataFileMat.is_open()) {
			string dataLineRawMat;
			while (getline(dataFileMat, dataLineRawMat)) {
				rawDataMat += dataLineRawMat;
				rawDataMat += "\n";
			}
			dataFileMat.close();
		}

		istringstream rawDataStreamMat(rawDataMat);
		string dataLineMat;
		while (std::getline(rawDataStreamMat, dataLineMat)) {
			if (dataLineMat.find("map_Kd ") != string::npos) {	// does this line have a texture map?
				textures.push_back(dataLineMat.substr(dataLineMat.find(" ") + 1));
			}
		}
	}

	(*obj).texture = new GLuint[textures.size()];		// Warning possible memory leak here - there is a new here, where is your delete?
	glCreateTextures(GL_TEXTURE_2D, textures.size(), (*obj).texture);

	for (unsigned int i = 0; i < textures.size(); i++) {
		readTexture(textures[i], (*obj).texture[i]);
	}

	cout << "done";
}

void readTexture(string name, GLuint textureName) {


	gli::texture tex = gli::load(name);
	if (tex.empty()) {
		cout << "Unable to load file " << name;
	}

	gli::gl texGl(gli::gl::PROFILE_GL33);
	gli::gl::format const texFormat = texGl.translate(tex.format(), tex.swizzles());
	//GLenum texTarget = texGl.translate(tex.target());

	// Load and create a texture 
	glBindTexture(GL_TEXTURE_2D, textureName); // All upcoming operations now have effect on this texture object

	glm::tvec3<GLsizei> const texExtent(tex.extent());
	GLsizei const texFaceTotal = static_cast<GLsizei>(tex.layers() * tex.faces());

	// Note: This only loads GL_TEXTURE_2D - for the complete code please visit >> http://gli.g-truc.net/
	glTexStorage2D(GL_TEXTURE_2D, static_cast<GLint>(tex.levels()), texFormat.Internal, texExtent.x, texExtent.y);

	for (std::size_t Layer = 0; Layer < tex.layers(); ++Layer) {
		for (std::size_t Face = 0; Face < tex.faces(); ++Face) {
			for (std::size_t Level = 0; Level < tex.levels(); ++Level) {
				glTextureSubImage2D(textureName, static_cast<GLint>(Level),
					0, 0,
					texExtent.x, texExtent.y,
					texFormat.External, texFormat.Type, tex.data(Layer, Face, Level));

			}
		}
	}

	// This only works for 2D Textures...
	// Set the texture wrapping parameters
	glTextureParameteri(textureName, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(textureName, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set texture filtering parameters
	glTextureParameteri(textureName, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(textureName, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);// Unbind texture when done, so we won't accidentily mess up our texture.



}

