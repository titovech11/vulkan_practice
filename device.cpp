#include "device.h"

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

void vkInit::log_device_properties(const vk::PhysicalDevice& device) {
	vk::PhysicalDeviceProperties properties = device.getProperties();

	/*
	* typedef struct VkPhysicalDeviceProperties {
		uint32_t                            apiVersion;
		uint32_t                            driverVersion;
		uint32_t                            vendorID;
		uint32_t                            deviceID;
		VkPhysicalDeviceType                deviceType;
		char                                deviceName[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE];
		uint8_t                             pipelineCacheUUID[VK_UUID_SIZE];
		VkPhysicalDeviceLimits              limits;
		VkPhysicalDeviceSparseProperties    sparseProperties;
		} VkPhysicalDeviceProperties;
	*/

	std::cout << "Phys device name: " << properties.deviceName << '\n';

	std::cout << "Phys device type: ";
	switch (properties.deviceType) {

	case (vk::PhysicalDeviceType::eCpu):
		std::cout << "CPU\n";
		break;
	case (vk::PhysicalDeviceType::eDiscreteGpu):
		std::cout << "Discrete GPU\n";
		break;
	case (vk::PhysicalDeviceType::eIntegratedGpu):
		std::cout << "Integrated GPU\n";
		break;
	case (vk::PhysicalDeviceType::eVirtualGpu):
		std::cout << "Virtual GPU\n";
		break;
	default:
		std::cout << "Other\n";
	}
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

vkInit::QueueFamilyIndices vkInit::findQueueFamilies(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface, bool debug) {
	QueueFamilyIndices indices;

	std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

	if (debug) {
		std::cout << "System can support " << queueFamilies.size() << " queue families\n";
	}
	int i = 0;
	for (const vk::QueueFamilyProperties& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
			indices.graphicsFamily = i;
			if (debug)
				std::cout << "Queue family " << i << " is suitable for graphics\n";
		}

		if (device.getSurfaceSupportKHR(i, surface)) {
			indices.presentFamily = i;
			if (debug)
				std::cout << "Queue family " << i << " is suitable for presenting\n";
		}

		if (indices.isComplete())
			break;

		i++;
	}

	return indices;
}

vk::Device vkInit::create_logical_device(const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface, const bool debug) {


	QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface, debug);
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

	vk::PhysicalDeviceFeatures deviceFeatures = vk::PhysicalDeviceFeatures();

	std::vector<const char* > enabledLayers;

	if (debug) {
		enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
	}

	vk::DeviceCreateInfo deviceInfo = vk::DeviceCreateInfo(
		vk::DeviceCreateFlags(), queueCreateInfo.size(), queueCreateInfo.data(), enabledLayers.size(), enabledLayers.data(),
		0, nullptr, &deviceFeatures
	);

	try {
		vk::Device device = physicalDevice.createDevice(deviceInfo);
		if (debug) {
			std::cout << "GPU has been succesfully abstracted\n";
		}
		return device;
	}
	catch(vk::SystemError err) {
		if (debug) {
			std::cout << "Device creation failed - " << err.what() << std::endl;
			return nullptr;
		}
	}
}

std::array<vk::Queue, 2> vkInit::get_queue(const vk::PhysicalDevice& physicalDevice, const vk::Device& device, const vk::SurfaceKHR& surface, const bool debug) {

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface, debug);

	return { {
			device.getQueue(indices.graphicsFamily.value(), 0),
			device.getQueue(indices.presentFamily.value(), 0)
		} };
}

bool vkInit::QueueFamilyIndices::isComplete() {
	return graphicsFamily.has_value() && presentFamily.has_value();
}
