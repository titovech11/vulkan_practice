#pragma once
#include "config.h"


class Engine {

public:
	const std::string app_name = "vulkan wnd";
	Engine();

	~Engine();

private:

	const bool debugMode = true;

	vk::Instance instance{ nullptr };
	vk::DebugUtilsMessengerEXT debugMessenger{ nullptr };
	vk::DispatchLoaderDynamic dldi;
	vk::SurfaceKHR surface;

	vk::PhysicalDevice physicalDevice{ nullptr };
	vk::Device device{ nullptr };
	vk::Queue graphicsQueue{ nullptr };
	vk::Queue presentQueue{ nullptr };

	int width{ 640 };
	int height{ 480 };
	GLFWwindow* window{ nullptr };

	void build_glfw_window();
	void make_instance();

	void make_device();
};