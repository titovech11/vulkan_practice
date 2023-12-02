#pragma once
#include "config.h"
#include "engine.h"
#include "scene.h"

class App {

private:
	Engine* graphicsEngine;
	GLFWwindow* window;
	Scene* scene;

	double lastTime, currentTime;
	int numFrames;
	float frameTime;
	std::string appName = "vulkan wnd";

	void build_glfw_window(int width, int height, std::string appName, bool debugMode);

	void calculateFrameRate();

public:
	App(int width, int height, std::string appName, bool debug);
	~App();
	void run();
};