#include <stdlib.h>
#include <stdio.h>
#include "gl.h"
#include "window.h"
#include "settings.h"
#include "dbg.h"

static GLFWwindow* window;

static void error_callback(int error, const char* description)
{
    log_err("glfw error %d : %s", error, description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void windowInit()
{
	glfwSetErrorCallback(error_callback);

	if(!glfwInit()) exit(EXIT_FAILURE);
	
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(settings.width, settings.height, "game", NULL, NULL);

	if(!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwSetKeyCallback(window, key_callback);
}

void windowExit()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

bool windowUpdate()
{
	glfwSwapBuffers(window);
	glfwPollEvents();

	return !glfwWindowShouldClose(window);
}
