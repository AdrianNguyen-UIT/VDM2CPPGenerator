#include "pch.h"
#include "GUI.h"
#include "Timer.h"
#define WIDTH 1600
#define HEIGHT 1000
#define TITLE "VDM2CPPGenerator"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void window_close_callback(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void GUIRender();

bool windowShouldClose = false;
VDMParser::Parser parser;

int main()
{
	Log::Init();

	LOG_TRACE("Application: Initializing glfw...");
	{
		Timer glfwTimer;
		if (glfwInit() == 0)
		{
			LOG_ERROR("Application: Fail");
			return 0;
		}
		glfwTimer.Stop();
		LOG_INFO("Application: Success");
		glfwTimer.Log();
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window;
	LOG_TRACE("Application: Creating {0} window...", TITLE);
	{
		Timer windowTimer;
		window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, nullptr, nullptr);
		if (window == nullptr)
		{
			LOG_ERROR("Application: Fail");
			glfwTerminate();
			return 0;
		}
		windowTimer.Stop();
		LOG_INFO("Application: Success");
		windowTimer.Log();
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetWindowCloseCallback(window, window_close_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSwapInterval(1);

	LOG_TRACE("Application: Initializing GLAD...");
	{
		Timer gladTimer;
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			LOG_ERROR("Application: Fail");
			return 0;
		}
		gladTimer.Stop();
		LOG_INFO("Application: Success");
		gladTimer.Log();
	}

	LOG_TRACE("Application: Initializing GUI...");
	{
		Timer guiTimer;
		GUI::Init(window);
		guiTimer.Stop();
		LOG_INFO("Application: Success");
		guiTimer.Log();
	}

	LOG_TRACE("Application: Render...");
	while (!windowShouldClose)
	{
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwPollEvents();

		GUI::Render(GUIRender);

		glfwSwapBuffers(window);
	}

	LOG_TRACE("Application: Closing window...");
	glfwDestroyWindow(window);
	glfwTerminate();
	GUI::Terminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void window_close_callback(GLFWwindow* window)
{
	windowShouldClose = true;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if ((mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL)
	{
		if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
			if ((mods & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT)
			{
				GUI::SaveInputFileAs(parser);
			}
			else if ((mods & GLFW_MOD_ALT) == GLFW_MOD_ALT)
			{
				GUI::SaveOutputFileAs(parser);
			}
			else
			{
				GUI::SaveInputFile(parser);
			}
		}
		
		if (key == GLFW_KEY_O && action == GLFW_PRESS)
		{
			GUI::OpenInputFile(parser);
		}

		if (key == GLFW_KEY_N && action == GLFW_PRESS)
		{
			GUI::NewInputAndOutput(parser);
		}
	}
}

void GUIRender()
{
	GUI::ShowGUI(parser, windowShouldClose);
}