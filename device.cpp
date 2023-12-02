#include "device.h"
#include "logging.h"
#include "queue_families.h"

bool vkInit::checkDeviceExtensionSupport(const vk::PhysicalDevice& device, const std::vector<const char*>& requestedExtensions, const bool debug) {

	std::set<std::string> requiredExtensions(requestedExtensions.begin(), requestedExtensions.end());


	for (vk::ExtensionProperties& extension : device.enumerateDeviceExtensionProperties()) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

bool vkInit::isSuitable(const vk::PhysicalDevice& device, const bool debug) {

	if (debug) {
		std::cout << "Checking if phys device is suitable\n";
	}

	const std::vector<const char*> requestedExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	if (debug) {
		std::cout << "We are requesting phys device extensions:\n";

		for (const char* extension : requestedExtensions) {
			std::cout << "\t\"" << extension << "\"\n";
		}

	}

	if (checkDeviceExtensionSupport(device, requestedExtensions, debug)) {

		if (debug) {
			std::cout << "Phys device can support the requested extensions!\n";
		}
	}
	else {

		if (debug) {
			std::cout << "Phys device can't support the requested extensions!\n";
		}

		return false;
	}
	return true;
}

vk::PhysicalDevice vkInit::choose_physical_device(vk::Instance& instance, const bool debug) {

	std::vector<vk::PhysicalDevice> availableDevices = instance.enumeratePhysicalDevices();

	if (debug) {
		std::cout << availableDevices.size() << " phys devices avaliable in the system \n";
	}

	for (const vk::PhysicalDevice& device : availableDevices) {
		if (debug) {
			log_device_properties(device);
		}
		if (isSuitable(device, debug)) {
			return device;
		}
	}

	return nullptr;
}

vk::Device vkInit::create_logical_device(const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface, const bool debug) {

	vkUtil::QueueFamilyIndices indices = vkUtil::findQueueFamilies(physicalDevice, surface, debug);
	std::vector<uint32_t> uniqueIndices;
	uniqueIndices.push_back(indices.graphicsFamily.value());
	if (indices.graphicsFamily.value() != indices.presentFamily.value()) {
		uniqueIndices.push_back(indices.presentFamily.value());
	}

	float queuePriority = 1.0f;

	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfo;
	for (uint32_t queueFamilyIndeces : uniqueIndices) {
		queueCreateInfo.push_back(
			vk::DeviceQueueCreateInfo(
				vk::DeviceQueueCreateFlags(), queueFamilyIndeces,
				1, &queuePriority
			)
		);
	}

	std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	vk::PhysicalDeviceFeatures deviceFeatures = vk::PhysicalDeviceFeatures();

	std::vector<const char* > enabledLayers;

	if (debug) {
		enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
	}

	vk::DeviceCreateInfo deviceInfo = vk::DeviceCreateInfo(
		vk::DeviceCreateFlags(), queueCreateInfo.size(), queueCreateInfo.data(), enabledLayers.size(), enabledLayers.data(),
		deviceExtensions.size(), deviceExtensions.data(), &deviceFeatures
	);

	try {
		vk::Device device = physicalDevice.createDevice(deviceInfo);
		if (debug) {
			std::cout << "GPU has been succesfully abstracted\n";
		}
		return device;
	}
	catch(vk::SystemError err) {
			std::cout << "Device creation failed - " << err.what() << std::endl;
			return nullptr;
	}
}

std::array<vk::Queue, 2> vkInit::get_queues(const vk::PhysicalDevice& physicalDevice, const vk::Device& device, const vk::SurfaceKHR& surface, const bool debug) {

	vkUtil::QueueFamilyIndices indices = vkUtil::findQueueFamilies(physicalDevice, surface, debug);

	return { {
			device.getQueue(indices.graphicsFamily.value(), 0),
			device.getQueue(indices.presentFamily.value(), 0)
		} };
}

