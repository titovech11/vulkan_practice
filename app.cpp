#include "app.h"

App::App(int width, int height, std::string appName, bool debug) {

	build_glfw_window(width, height, appName, debug);

	graphicsEngine = new Engine(width, height, window, debug);

	scene = new Scene();
}

void App::build_glfw_window(int width, int height, std::string appName, bool debugMode) {

	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	if (window = glfwCreateWindow(width, height, appName.c_str(), nullptr, nullptr)) {
		if (debugMode) {
			std::cout << "build_glfw_window succeed  " << appName << ", width: " << width << ", height : " << height << '\n';
		}
	}
	else {
		if (debugMode) {
			std::cout << "build_glfw_window  failed\n";
		}
	}
}

void App::run() {

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		graphicsEngine->render(scene);
		calculateFrameRate();
	}
}

void App::calculateFrameRate() {
	currentTime = glfwGetTime();
	double delta = currentTime - lastTime;

	if (delta >= 1) {
		int framerate{ std::max(1, int(numFrames / delta)) };
		std::stringstream title;
		title << " " << framerate << " fps.";
		glfwSetWindowTitle(window, title.str().c_str());
		lastTime = currentTime;
		numFrames = -1;
		frameTime = float(1000.0 / framerate);
	}

	++numFrames;
}

App::~App() {
	delete graphicsEngine;
	delete scene;
}