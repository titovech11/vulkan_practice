#pragma once
#include "config.h"
#include "logging.h"
#include "queue_families.h"
#include "frame.h"

namespace vkInit {

	struct SwapChainSupportDetails {
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> presentModes;
	};

	struct SwapChainBundle {
		vk::SwapchainKHR swapchain;
		std::vector<vkUtil::SwapChainFrame> frames;
		vk::Format format;
		vk::Extent2D extent;
	};

	SwapChainSupportDetails query_swapchain_support(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface, const bool debug);

	vk::SurfaceFormatKHR choose_swapchain_surface_format(const std::vector<vk::SurfaceFormatKHR>& formats);

	vk::PresentModeKHR choose_swapchain_present_mode(const std::vector<vk::PresentModeKHR>& presentModes);

	vk::Extent2D choose_swapchain_extent(const uint32_t width, const uint32_t height, const vk::SurfaceCapabilitiesKHR& capabilities);

	SwapChainBundle create_swapchain(const vk::Device& logicalDevice, const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface, const int width, const int height, const bool debug);
}
