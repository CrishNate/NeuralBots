#include <GLFW/glfw3.h>
#include <GL\freeglut.h>
#include <stdlib.h>
#include <stdio.h>
#include "World.h"
#include "noise.h"
#include <time.h>
#include "Bot.h"

#define FPS 60

// fields
Vector2D g_GrabPos;
PerlinNoise* g_pNouse;
bool g_Grabbed = false;

World* g_pMainWorld;
Bot* g_pBot;
Camera g_Camera = Camera(Vector2D(WORLD_SIZE_X / 2.0f, WORLD_SIZE_Y / 2.0f), 1280, 720);

time_t g_LastTime;

void run(GLFWwindow* pWindow)
{
	float ratio;
	int width, height;
	glfwGetFramebufferSize(pWindow, &width, &height);
	float zoom = g_Camera.zoom;

	// Changing width/height of window
	ratio = width / (float)height;
	glViewport(0, 0, width, height);
	g_Camera.height = height;
	g_Camera.widht = width;
	
	// clearing background and filling with color
	glClearColor(0, 0, 0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, 1.f, -1.f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glLineWidth(8);
	glColor3f(1.0f, 0.0f, 0.0f);

	//if (g_SelectedCreature)
	//{
	//	const std::vector<CellCreature*>& ccreatures = g_pMainWorld->GetCellCreatures();
	//	if (std::find(ccreatures.begin(), ccreatures.end(), g_SelectedCreature) == ccreatures.end())
	//		g_SelectedCreature = NULL;
	//}

	//if (g_SelectedCreature)
	//{
	//	DrawFilledCircle(g_SelectedCreature->GetPosition().x, g_SelectedCreature->GetPosition().y, g_SelectedCreature->GetRadius() + 20, RGBColor(0, 0, 0), g_Camera);
	//}

	g_pMainWorld->Draw(g_Camera);

	// running
	g_pMainWorld->Step();

	if (g_pBot)
	{
		const std::vector<Bot*>& bots = g_pMainWorld->GetBots();
		if (std::find(bots.begin(), bots.end(), g_pBot) == bots.end())
			g_pBot = NULL;
	}

	// Drawing Info
	if (g_pBot)
	{
		int panelW = 300;
		int panelPadding = 20;
		DrawFilledRect(0, 0, panelW, height, RGBColor(110, 130, 150));
		DrawFilledRect(
			panelPadding,
			panelPadding,
			panelW - panelPadding * 2,
			panelW - panelPadding * 2,
			RGBColor(55, 65, 75)
			);

		// Drawing brain
		NeuralNetwork* pBrain = g_pBot->GetBrain();
		pBrain->Draw(150, height - pBrain->GetDrawHeight() / 2);
	}

	glfwSwapBuffers(pWindow);
	glfwPollEvents();
}

static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if (action == GLFW_PRESS)
		{
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);

			g_Grabbed = true;
			g_GrabPos = Vector2D(xpos, ypos);
		}
		else if (action == GLFW_RELEASE)
		{
			g_Grabbed = false;
			g_GrabPos = Vector2D(0, 0);
		}
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		xpos = g_Camera.pos.x + xpos / g_Camera.zoom;
		ypos = g_Camera.pos.y + ypos / g_Camera.zoom;
		Vector2D mousePos = Vector2D(xpos, ypos);
		const std::vector<Bot*>& bots = g_pMainWorld->GetBots();
		Bot* pPrevBot = g_pBot;

		int dist = bots[0]->GetPosition().Distance(mousePos);
		g_pBot = bots[0];

		for (int i = 1; i < bots.size(); i++)
		{
			if (dist > bots[i]->GetPosition().Distance(mousePos))
			{
				dist = bots[i]->GetPosition().Distance(mousePos);
				g_pBot = bots[i];
			}
		}

		if (pPrevBot == g_pBot)
			g_pBot = NULL;
	}

}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (g_Grabbed)
	{
		Vector2D mousePos = Vector2D(xpos, ypos);
		Vector2D dir = (mousePos - g_GrabPos);
		g_Camera.center.x -= dir.x / g_Camera.zoom;
		g_Camera.center.y -= dir.y / g_Camera.zoom;

		g_Camera.pos.x = g_Camera.center.x - (g_Camera.widht / 2) / g_Camera.zoom;
		g_Camera.pos.y = g_Camera.center.y - (g_Camera.height / 2) / g_Camera.zoom;
		g_GrabPos = mousePos;
	}
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (yoffset == 1)
	{
		g_Camera.zoom *= 1.1;
	}
	else if (yoffset == -1)
	{
		g_Camera.zoom /= 1.1;
	}

	g_Camera.pos.x = g_Camera.center.x - (g_Camera.widht / 2) / g_Camera.zoom;
	g_Camera.pos.y = g_Camera.center.y - (g_Camera.height / 2) / g_Camera.zoom;
}

int main(void)
{
	// Setting random seed
	srand(static_cast <unsigned> (time(0)));

	g_pNouse = new PerlinNoise(0, 0.01, 10, 1, frand(1, 10000.0f));

	//g_Solver = new PhysSolver(100);

	// Creating Window
	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		exit(EXIT_FAILURE);
	window = glfwCreateWindow(1280, 720, "Neural Bots", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	// Draw Loading screen background
	glClearColor(0, 0, 0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	//DrawTextQL("Loading", 1280 / 2.0f, 720 / 2.0f, 0, RGBColor(255, 255, 255));
	glfwSwapBuffers(window);
	glfwPollEvents();

	// Creating World
	g_pMainWorld = new World();

	// Handling
	while (!glfwWindowShouldClose(window))
	{
		if (clock() > g_LastTime)
		{
			g_LastTime = clock() + 1000 / FPS;

			run(window);
		}
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}