#include "engine.h"
#include "instance.h"
#include "logging.h"
#include "device.h"

Engine::Engine() {

	if (debugMode) {
		std::cout << "engine ctor\n";
	}

	build_glfw_window();
	make_instance();
	make_device();
}

void Engine::build_glfw_window() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	if (window = glfwCreateWindow(width, height, app_name.c_str(), nullptr, nullptr)) {
		if (debugMode) {
			std::cout << "build_glfw_window succeed  "<< app_name <<", width: " << width << ", height : " << height << '\n';
		}
	}
	else {
		if (debugMode) {
			std::cout << "build_glfw_window  failed\n";
		}
	}
}

void Engine::make_instance()
{
	instance = vkInit::make_instance(debugMode, app_name.c_str());
	dldi = vk::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr);

	if (debugMode) {
		debugMessenger = vkInit::make_debug_messenger(instance, dldi);
	}
	VkSurfaceKHR c_style_surface;
	
	if (glfwCreateWindowSurface(instance, window, nullptr, &c_style_surface) != VK_SUCCESS) {
		if (debugMode) {
			std::cout << "Failed to abstract glfw surface for vulcan \n";
		}
	}
	else if (debugMode) {
		std::cout << "Successfully abstract glfw surface for vulcan \n";
	}
	surface = c_style_surface;
}

void Engine::make_device()
{
	physicalDevice = vkInit::choose_physical_device(instance, debugMode);
	device = vkInit::create_logical_device(physicalDevice, surface, debugMode);
	std::array<vk::Queue, 2> queues = vkInit::get_queue(physicalDevice, device, surface, debugMode);
	graphicsQueue = queues[0];
	presentQueue = queues[1];
}


Engine::~Engine() {

	if (debugMode) {
		std::cout << "engine dtor\n";
	}
	device.destroy();

	instance.destroySurfaceKHR(surface);
	if (debugMode) {
		instance.destroyDebugUtilsMessengerEXT(debugMessenger, nullptr, dldi);
	}
	instance.destroy();

	glfwTerminate();
}
