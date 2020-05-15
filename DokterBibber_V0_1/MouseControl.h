#include <glm\glm.hpp>

struct GLFWwindow;

#pragma once
class MouseControl
{
public:
	MouseControl(GLFWwindow* window, const int height, const int width, const float scaler = 1);
	~MouseControl() = default;
	
	double getMouseXPos();
	double getMouseYPos();

	void updateMousePos(GLFWwindow* window);
	void updateMouseMode(GLFWwindow* window, int glMouseMode);

private:
	double xPos;
	double yPos;

	int screenHeigth;
	int screenWidth;
};

