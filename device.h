#pragma once
#include "config.h"

namespace vkInit {

	bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device, const std::vector<const char*>& requestedExtensions, const bool debug);

	bool isSuitable(const vk::PhysicalDevice& device, const bool debug);

	vk::PhysicalDevice choose_physical_device(vk::Instance& instance, const bool debug);

	vk::Device create_logical_device(const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface, const bool device);

	std::array<vk::Queue, 2> get_queues(const vk::PhysicalDevice& physicalDevice, const vk::Device& device, const vk::SurfaceKHR& surface, const bool debug);
}