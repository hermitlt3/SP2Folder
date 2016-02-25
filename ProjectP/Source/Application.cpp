
#include "Application.h"

//Include GLEW
#include <GL/glew.h>

//Include GLFW
#include <GLFW/glfw3.h>

//Include the standard C++ headers
#include <stdio.h>
#include <stdlib.h>

#include "SP2.h"
#include "Planet1.h"
#include "Planet2.h"
#include "Planet3.h"
#include "Planet4.h"
#include "Planet5.h"
#include "Gamemode.h"
#include "MainMenu.h"

GLFWwindow* m_window;
const unsigned char FPS = 60; // FPS of this game
const unsigned int frameTime = 1000 / FPS; // time for each frame
//Define an error callback
static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
	_fgetchar();
}

//Define the key input callback
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

bool Application::IsKeyPressed(unsigned short key)
{
    return ((GetAsyncKeyState(key) & 0x8001) != 0);
}

Application::Application()
{
}

Application::~Application()
{
}

void resize_callback(GLFWwindow* window, int w, int h)
{
	glViewport(10, 10, w, h); //update opengl the new window size
}
void Application::Init()
{
	//Set the error callback
	glfwSetErrorCallback(error_callback);

	//Initialize GLFW
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	//Set the GLFW window creation hints - these are optional
	glfwWindowHint(GLFW_SAMPLES, 4); //Request 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Request a specific OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //Request a specific OpenGL version
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 


	//Create a window and create its OpenGL context
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	m_window = glfwCreateWindow(mode->width, mode->height, "Computer Graphics", NULL, NULL);
	glfwSetWindowSizeCallback(m_window, resize_callback);



	//If the window couldn't be created
	if (!m_window)
	{
		fprintf( stderr, "Failed to open GLFW window.\n" );
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//This function makes the context of the specified window current on the calling thread. 
	glfwMakeContextCurrent(m_window);

	//Sets the key callback
	//glfwSetKeyCallback(m_window, key_callback);

	glewExperimental = true; // Needed for core profile
	//Initialize GLEW
	GLenum err = glewInit();

	//If GLEW hasn't initialized
	if (err != GLEW_OK) 
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		//return -1;
	}
}

void Application::Run()
{
	//Main Loop
	Scene *scene0 = new MAINMENU();
	Scene *scene1 = new SP2();
	Scene *scene2 = new PLANET1();
	Scene *scene3 = new PLANET2();
	Scene *scene4 = new PLANET3();
	Scene *scene5 = new PLANET4();
	Scene *scene6 = new PLANET5();

	Scene *currScene = scene0;

	scene1->Init();
	scene2->Init();
	scene3->Init();
	scene4->Init();
	scene5->Init();
	scene6->Init();

	m_timer.startTimer();    // Start timer to calculate how long it takes to render this frame
	while ((!glfwWindowShouldClose(m_window) && !(c_UserInterface::GetEnum()->quitGame)))
	{
		if (IsKeyPressed('P'))
		{
			currScene = scene0;
		}
		if (GameMode::GetInstance()->gameState == 1)
		{
			currScene = scene1;
		}

		else if (GameMode::GetInstance()->gameState == 2)
		{
			currScene = scene2;
		}

		else if (GameMode::GetInstance()->gameState == 3)
		{
			currScene = scene3;
		}

		else if (GameMode::GetInstance()->gameState == 4)
		{
			currScene = scene4;
		}

		else if (GameMode::GetInstance()->gameState == 5)
		{
			currScene = scene5;
		}

		else if (GameMode::GetInstance()->gameState == 6)
		{
			currScene = scene6;
			GameMode::GetInstance()->gameState = 0;
		}
		currScene->Update(m_timer.getElapsedTime());
		currScene->Render();
		//Swap buffers
		glfwSwapBuffers(m_window);
		//Get and organize events, like keyboard and mouse input, window resizing, etc...
		glfwPollEvents();
        m_timer.waitUntil(frameTime);       // Frame rate limiter. Limits each frame to a specified time in ms.   

	} //Check if the ESC key had been pressed or if the window had been closed
	
	currScene->Exit();
	delete currScene;
}

void Application::Exit()
{
	//Close OpenGL window and terminate GLFW
	glfwDestroyWindow(m_window);
	//Finalize and clean up GLFW
	glfwTerminate();
}
