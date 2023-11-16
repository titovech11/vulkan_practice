#pragma once
#include "config.h"

namespace vkInit {

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete();
	};

	bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device, const std::vector<const char*>& requestedExtensions, const bool debug);

	bool isSuitable(const vk::PhysicalDevice& device, const bool debug);

	void log_device_properties(const vk::PhysicalDevice& device);

	vk::PhysicalDevice choose_physical_device(vk::Instance& instance, const bool debug);

	QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface, const bool debug);

	vk::Device create_logical_device(const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface, const bool device);

	std::array<vk::Queue, 2> get_queue(const vk::PhysicalDevice& physicalDevice, const vk::Device& device, const vk::SurfaceKHR& surface, const bool debug);
}