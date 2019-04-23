#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "glad.h"

// GLM headers
#define GLM_FORCE_PURE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

// ( ͡° ͜ʖ ͡°) mesh data
#include "lenny.h"

#ifndef __SWITCH__
#define ASSET(_str) "./resources/" _str
#else
#define ASSET(_str) "romfs:/" _str
#endif

constexpr uint32_t WINDOW_WIDTH = 1280;
constexpr uint32_t WINDOW_HEIGHT = 720;
constexpr const char* WINDOW_TITLE = "glLenny";

constexpr uint32_t NUMOBJECTS = 64;
constexpr auto TAU = glm::two_pi<float>();

//-----------------------------------------------------------------------------
// Main program
//-----------------------------------------------------------------------------

static GLuint loadShaderProgram(GLenum type, const char* src_path)
{
	GLint success;
	GLchar msg[512];

	FILE* f = fopen(src_path, "rb");
	if (!f)
	{
		printf("Could not open shader: %s\n", src_path);
		return 0;
	}

	fseek(f, 0, SEEK_END);
	size_t shader_size = ftell(f);
	rewind(f);

	char* source = new char[shader_size+1];
	fread(source, 1, shader_size, f);
	source[shader_size] = 0;
	fclose(f);

	GLuint handle = glCreateShaderProgramv(type, 1, &source);
	delete[] source;

	glGetProgramiv(handle, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		glGetProgramInfoLog(handle, sizeof(msg), nullptr, msg);
		printf("Shader error: %s\n", msg);
		glDeleteProgram(handle);
		handle = 0;
	}

	return handle;
}

// Per-instance data
struct Instance
{
	glm::mat4 mdlMtx;
};

namespace Uniforms
{
	struct Transformation
	{
		glm::mat4 mdlvMtx;
		glm::mat4 projMtx;
	};

	struct Lighting
	{
		glm::vec4 lightPos;
		glm::vec3 ambient;
		float _padding0;
		glm::vec3 diffuse;
		float _padding1;
		glm::vec3 specular;
		float shininess;
	};
}

static GLuint s_vshProgram, s_fshProgram;
static GLuint s_simplePipeline;
static GLuint s_vao, s_vbo, s_instance_vbo;
static GLuint s_ubos[2];

static Uniforms::Transformation s_transformation;
static Uniforms::Lighting s_lighting;

static void sceneInit()
{
	// Load our shaders
	s_vshProgram = loadShaderProgram(GL_VERTEX_SHADER, ASSET("shaders/simple_vsh.glsl"));
	s_fshProgram = loadShaderProgram(GL_FRAGMENT_SHADER, ASSET("shaders/simple_fsh.glsl"));

	// Create a program pipeline and attach the programs to their respective stages
	glGenProgramPipelines(1, &s_simplePipeline);
	glUseProgramStages(s_simplePipeline, GL_VERTEX_SHADER_BIT,   s_vshProgram);
	glUseProgramStages(s_simplePipeline, GL_FRAGMENT_SHADER_BIT, s_fshProgram);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glGenVertexArrays(1, &s_vao);
	glGenBuffers(1, &s_vbo);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(s_vao);

	glBindBuffer(GL_ARRAY_BUFFER, s_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lennyVertices), lennyVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(lennyVertex), (void*)offsetof(lennyVertex, x));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(lennyVertex), (void*)offsetof(lennyVertex, nx));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &s_instance_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, s_instance_vbo);

	// Generate the per-instance data: the instances will form a sine wave.
	Instance* instances = new Instance[NUMOBJECTS];
	for (size_t i = 0; i < NUMOBJECTS; i ++)
	{
		float x = float(i) / (NUMOBJECTS-1);
		float a = x*2.0f-1.0f;
		instances[i].mdlMtx = glm::translate(glm::mat4{1.0f}, glm::vec3{a*4.0f, 1.5f*sinf(x*TAU), a});
		instances[i].mdlMtx = glm::scale(instances[i].mdlMtx, glm::vec3{2.0f});
	}

	// Upload the instance data
	glBufferData(GL_ARRAY_BUFFER, sizeof(Instance)*NUMOBJECTS, instances, GL_STATIC_DRAW);
	delete[] instances;

	// Set up per-instance attributes
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Instance), (void*)(offsetof(Instance, mdlMtx)+0*sizeof(glm::vec4)));
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Instance), (void*)(offsetof(Instance, mdlMtx)+1*sizeof(glm::vec4)));
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Instance), (void*)(offsetof(Instance, mdlMtx)+2*sizeof(glm::vec4)));
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Instance), (void*)(offsetof(Instance, mdlMtx)+3*sizeof(glm::vec4)));
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// Create the uniform buffers & configure binding points
	glGenBuffers(2, s_ubos);
	glBindBuffer(GL_UNIFORM_BUFFER, s_ubos[0]);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(s_transformation), nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, s_ubos[0]);
	glBindBuffer(GL_UNIFORM_BUFFER, s_ubos[1]);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(s_lighting), nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, s_ubos[1]);

	// Initialize uniforms
	s_transformation.projMtx = glm::mat4();
	s_lighting.lightPos = glm::vec4(0.0f, 0.0f, -0.5f, 1.0f);
	s_lighting.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
	s_lighting.diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
	s_lighting.specular = glm::vec3(0.5f, 0.5f, 0.5f);
	s_lighting.shininess = 20.0f;

	// Update lighting uniform block
	glBindBuffer(GL_UNIFORM_BUFFER, s_ubos[1]);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(s_lighting), &s_lighting);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

static double s_updTime = 0.0;
static float s_cameraAngle = 0.0f;
static glm::vec3 s_cameraPos{0.0f, 0.0f, 3.0f};

template <typename T>
static inline T fract(T x)
{
	return x - std::floor(x);
}

static void sceneUpdate(const GLFWgamepadstate& gamepad)
{
	double curTime = glfwGetTime();
	float deltaTime = curTime - s_updTime;
	s_updTime = curTime;

	const bool left_pressed  = gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT]  == GLFW_PRESS;
	const bool right_pressed = gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] == GLFW_PRESS;
	const bool up_pressed    = gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP]    == GLFW_PRESS;
	const bool down_pressed  = gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN]  == GLFW_PRESS;

	const float axis_x =  gamepad.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
	const float axis_y = -gamepad.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
	const float axis_magnitude = sqrtf(axis_x*axis_x + axis_y*axis_y);
	//const float axis_angle = atan2f(axis_y, axis_x);

	float advance_change = 0.0f;
	float angle_change = 0.0f;
	if (up_pressed)
		advance_change += 1.0f;
	else if (down_pressed)
		advance_change -= 1.0f;
	if (left_pressed)
		angle_change -= 1.0f;
	else if (right_pressed)
		angle_change += 1.0f;

	if (axis_magnitude >= 0.1f)
	{
		advance_change += axis_y;
		angle_change += axis_x;
	}

	advance_change = glm::clamp(advance_change, -1.0f, +1.0f);
	angle_change   = glm::clamp(angle_change,   -1.0f, +1.0f);

	s_cameraAngle = fract(s_cameraAngle + angle_change*deltaTime/4);
	s_cameraPos += advance_change * deltaTime * glm::rotate(glm::vec3{0.0f, 0.0f, -1.0f}, s_cameraAngle * TAU, glm::vec3{0.0f, -1.0f, 0.0f});

	// Update modelview matrix
	glm::mat4 mdlvMtx{1.0};
	mdlvMtx = glm::rotate(mdlvMtx, s_cameraAngle * TAU, glm::vec3{0.0f, 1.0f, 0.0f});
	mdlvMtx = glm::translate(mdlvMtx, -s_cameraPos);
	s_transformation.mdlvMtx = mdlvMtx;
}

static void sceneRender()
{
	glClearColor(0x68/255.0f, 0xB0/255.0f, 0xD8/255.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Update transformation uniform block
	glBindBuffer(GL_UNIFORM_BUFFER, s_ubos[0]);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(s_transformation), &s_transformation);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Draw our ( ͡° ͜ʖ ͡°) world
	glBindProgramPipeline(s_simplePipeline);
	glBindVertexArray(s_vao); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
	glDrawArraysInstanced(GL_TRIANGLES, 0, lennyVerticesCount, NUMOBJECTS);
}

static void sceneExit()
{
	glDeleteBuffers(2, s_ubos);
	glDeleteBuffers(1, &s_instance_vbo);
	glDeleteBuffers(1, &s_vbo);
	glDeleteVertexArrays(1, &s_vao);
	glDeleteProgramPipelines(1, &s_simplePipeline);
	glDeleteProgram(s_fshProgram);
	glDeleteProgram(s_vshProgram);
}

static void windowFramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	if (!width || !height)
		return;

	printf("window size: %dx%d\n", width, height);
	glViewport(0, 0, width, height);
	s_transformation.projMtx = glm::perspective(glm::radians(40.0f), (float)width / (float)height, 0.01f, 1000.0f);
}

static void windowKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for toggle-fullscreen combo
	if (key == GLFW_KEY_ENTER && mods == GLFW_MOD_ALT && action == GLFW_PRESS)
	{
		static int saved_x, saved_y, saved_width, saved_height;

		if (!glfwGetWindowMonitor(window))
		{
			// Back up window position/size
			glfwGetWindowPos(window, &saved_x, &saved_y);
			glfwGetWindowSize(window, &saved_width, &saved_height);

			// Switch to fullscreen mode
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
		}
		else
		{
			// Switch back to windowed mode
			glfwSetWindowMonitor(window, nullptr, saved_x, saved_y, saved_width, saved_height, GLFW_DONT_CARE);
		}
	}
}

static void joystickCallback(int jid, int event)
{
	if (event == GLFW_CONNECTED)
	{
		printf("Joystick %d connected\n", jid);
		if (glfwJoystickIsGamepad(jid))
			printf("Joystick %d is gamepad: \"%s\"\n", jid, glfwGetGamepadName(jid));
	}
	else if (event == GLFW_DISCONNECTED)
		printf("Joystick %d disconnected\n", jid);
}

static inline bool windowMainLoop(GLFWwindow* window)
{
	bool is_active;
	do
	{
		is_active = !glfwGetWindowAttrib(window, GLFW_ICONIFIED);
		if (is_active)
			glfwPollEvents();
		else
			glfwWaitEvents();
		if (glfwWindowShouldClose(window))
			return false;
	} while (!is_active);
	return true;
}

int main(int argc, char* argv[])
{
	glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_FALSE);
	if (!glfwInit())
	{
		printf("glfw: failed to initialize\n");
		return EXIT_FAILURE;
	}

	// Create window
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
	if (!window)
	{
		printf("glfw: failed to create window\n");
		glfwTerminate();
		return EXIT_FAILURE;
	}

	// Configure window
	glfwSwapInterval(1);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, windowFramebufferSizeCallback);
	glfwSetKeyCallback(window, windowKeyCallback);
	glfwSetJoystickCallback(joystickCallback);

	// Load OpenGL routines using glad
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	printf("GL Vendor: %s\n", glGetString(GL_VENDOR));
	printf("GL Renderer: %s\n", glGetString(GL_RENDERER));
	printf("GL Version: %s\n", glGetString(GL_VERSION));

	if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
	{
		GLFWgamepadstate state;
		printf("Gamepad detected: %s\n", glfwGetGamepadName(GLFW_JOYSTICK_1));
		glfwGetGamepadState(GLFW_JOYSTICK_1, &state);
	}

	// Initialize our scene
	sceneInit();
	windowFramebufferSizeCallback(window, WINDOW_WIDTH, WINDOW_HEIGHT);
	glfwSetTime(0.0);

	while (windowMainLoop(window))
	{
		// Read gamepad
		GLFWgamepadstate gamepad = {};
		if (!glfwGetGamepadState(GLFW_JOYSTICK_1, &gamepad))
		{
			// Gamepad not available, so let's fake it with keyboard
			gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT]  = glfwGetKey(window, GLFW_KEY_LEFT);
			gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] = glfwGetKey(window, GLFW_KEY_RIGHT);
			gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP]    = glfwGetKey(window, GLFW_KEY_UP);
			gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN]  = glfwGetKey(window, GLFW_KEY_DOWN);
			gamepad.buttons[GLFW_GAMEPAD_BUTTON_START]      = glfwGetKey(window, GLFW_KEY_ESCAPE);
		}

		// Exit by pressing Start (aka Plus)
		if (gamepad.buttons[GLFW_GAMEPAD_BUTTON_START] == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			continue;
		}

		// Update our scene
		sceneUpdate(gamepad);

		// Render stuff!
		sceneRender();
		glfwSwapBuffers(window);
	}

	// Deinitialize our scene
	sceneExit();
	glfwTerminate();
	return EXIT_SUCCESS;
}
