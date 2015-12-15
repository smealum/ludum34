#include <stdlib.h>
#include <stdio.h>
#include "gl.h"
#include "window.h"
#include "settings.h"
#include "input.h"
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
	
	// glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(settings.width, settings.height, "qBen", NULL, NULL);

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

	int full_width, full_height;
	glfwGetFramebufferSize(window, &full_width, &full_height);
	
	int real_width = full_width, real_height = full_height;
	float forced_aspect = 4.0f / 3.0f;

	if(full_width < full_height)
	{
		real_width = full_width;
		real_height = real_width / forced_aspect;
	}else{
		real_height = full_height;
		real_width = real_height * forced_aspect;
	}

	settings.setResolution(real_width, real_height);

	int diff_width = full_width - real_width, diff_height = full_height - real_height;
	glViewport(diff_width / 2, diff_height / 2, real_width, real_height);

	Input::update(window);

	return !glfwWindowShouldClose(window);
}
