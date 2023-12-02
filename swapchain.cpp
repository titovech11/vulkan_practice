#include "swapchain.h"

vkInit::SwapChainSupportDetails vkInit::query_swapchain_support(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface, const bool debug) {

	SwapChainSupportDetails support;
	/*
	* typedef struct VkSurfaceCapabilitiesKHR {
		uint32_t                         minImageCount;
		uint32_t                         maxImageCount;
		VkExtent2D                       currentExtent;
		VkExtent2D                       minImageExtent;
		VkExtent2D                       maxImageExtent;
		uint32_t                         maxImageArrayLayers;
		VkSurfaceTransformFlagsKHR       supportedTransforms;
		VkSurfaceTransformFlagBitsKHR    currentTransform;
		VkCompositeAlphaFlagsKHR         supportedCompositeAlpha;
		VkImageUsageFlags                supportedUsageFlags;
	} VkSurfaceCapabilitiesKHR;
	*/
	support.capabilities = device.getSurfaceCapabilitiesKHR(surface);
	support.formats = device.getSurfaceFormatsKHR(surface);
	support.presentModes = device.getSurfacePresentModesKHR(surface);

	if (debug) {
		std::cout << "Swapchain can support the following surface capabilities:\n";

		std::cout << "\tminimum image count: " << support.capabilities.minImageCount << '\n';
		std::cout << "\tmaximum image count: " << support.capabilities.maxImageCount << '\n';

		std::cout << "\tcurrent extent: \n";

		std::cout << "\t\twidth: " << support.capabilities.currentExtent.width << '\n';
		std::cout << "\t\theight: " << support.capabilities.currentExtent.height << '\n';

		std::cout << "\tminimum supported extent: \n";
		std::cout << "\t\twidth: " << support.capabilities.minImageExtent.width << '\n';
		std::cout << "\t\theight: " << support.capabilities.minImageExtent.height << '\n';

		std::cout << "\tmaximum supported extent: \n";
		std::cout << "\t\twidth: " << support.capabilities.maxImageExtent.width << '\n';
		std::cout << "\t\theight: " << support.capabilities.maxImageExtent.height << '\n';

		std::cout << "\tmaximum image array layers: " << support.capabilities.maxImageArrayLayers << '\n';


		std::cout << "\tsupported transforms:\n";
		std::vector<std::string> stringList = log_transform_bits(support.capabilities.supportedTransforms);
		for (std::string line : stringList) {
			std::cout << "\t\t" << line << '\n';
		}
		std::cout << "\tcurrent transform:\n";
		stringList = log_transform_bits(support.capabilities.currentTransform);
		for (std::string line : stringList) {
			std::cout << "\t\t" << line << '\n';
		}

		std::cout << "\tsupported alpha operations:\n";
		stringList = log_alpha_composite_bits(support.capabilities.supportedCompositeAlpha);
		for (std::string line : stringList) {
			std::cout << "\t\t" << line << '\n';
		}

		std::cout << "\tsupported image usage:\n";
		stringList = log_image_usage_bits(support.capabilities.supportedUsageFlags);
		for (std::string line : stringList) {
			std::cout << "\t\t" << line << '\n';
		}

		for (vk::SurfaceFormatKHR supportedFormat : support.formats) {
			/*
			* typedef struct VkSurfaceFormatKHR {
				VkFormat           format;
				VkColorSpaceKHR    colorSpace;
			} VkSurfaceFormatKHR;
			*/

			std::cout << "supported pixel format: " << vk::to_string(supportedFormat.format) << '\n';
			std::cout << "supported color space: " << vk::to_string(supportedFormat.colorSpace) << '\n';
		}

		for (vk::PresentModeKHR presentMode : support.presentModes) {
			std::cout << '\t' << log_present_mode(presentMode) << '\n';
		}
	}

	return support;
}

vk::SurfaceFormatKHR vkInit::choose_swapchain_surface_format(const std::vector<vk::SurfaceFormatKHR>& formats) {

	for (vk::SurfaceFormatKHR format : formats) {
		if (vk::Format::eB8G8R8A8Unorm == format.format && vk::ColorSpaceKHR::eSrgbNonlinear == format.colorSpace) {
			return format;
		}
	}
	return formats[0];
}

vk::PresentModeKHR vkInit::choose_swapchain_present_mode(const std::vector<vk::PresentModeKHR>& presentModes) {

	for (vk::PresentModeKHR presentMode : presentModes) {
		if (vk::PresentModeKHR::eMailbox == presentMode) {
			return presentMode;
		}
	}

	return vk::PresentModeKHR::eFifo;
}

vk::Extent2D vkInit::choose_swapchain_extent(const uint32_t width, const uint32_t height, const vk::SurfaceCapabilitiesKHR& capabilities) {
	if (UINT32_MAX != capabilities.currentExtent.width) {
		return capabilities.currentExtent;
	}
	else {
		vk::Extent2D extent = { width, height };

		extent.width = std::min(
			capabilities.maxImageExtent.width,
			std::max(capabilities.minImageExtent.width, width)
		);

		extent.height = std::min(
			capabilities.maxImageExtent.height,
			std::max(capabilities.minImageExtent.height, height)
		);

		return extent;
	}
}

vkInit::SwapChainBundle vkInit::create_swapchain(const vk::Device& logicalDevice, const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface, const int width, const int height, const bool debug) {

	SwapChainSupportDetails support = query_swapchain_support(physicalDevice, surface, debug);

	vk::SurfaceFormatKHR format = choose_swapchain_surface_format(support.formats);

	vk::PresentModeKHR presentMode = choose_swapchain_present_mode(support.presentModes);

	vk::Extent2D extent = choose_swapchain_extent(width, height, support.capabilities);

	uint32_t imageCount = std::min(
		support.capabilities.maxImageCount,
		support.capabilities.minImageCount + 1
	);

	vk::SwapchainCreateInfoKHR createInfo = vk::SwapchainCreateInfoKHR(
		vk::SwapchainCreateFlagsKHR(), surface, imageCount, format.format, format.colorSpace,
		extent, 1, vk::ImageUsageFlagBits::eColorAttachment
	);

	vkUtil::QueueFamilyIndices indices = vkUtil::findQueueFamilies(physicalDevice, surface, debug);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily.value() != indices.presentFamily.value()) {
		createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = vk::SharingMode::eExclusive;
	}

	createInfo.preTransform = support.capabilities.currentTransform;
	createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = vk::SwapchainKHR(nullptr);

	SwapChainBundle bundle{};
	try {
		bundle.swapchain = logicalDevice.createSwapchainKHR(createInfo);
	}
	catch (vk::SystemError err) {
		std::cout << "failed to create swapchain - " << err.what() << std::endl;
		throw std::runtime_error("failed to create swapchain");
	}

	std::vector<vk::Image> images = logicalDevice.getSwapchainImagesKHR(bundle.swapchain);
	bundle.frames.resize(images.size());

	for (size_t i = 0; i != images.size(); ++i) {
		vk::ImageViewCreateInfo createInfo = {};
		createInfo.image = images[i];
		createInfo.viewType = vk::ImageViewType::e2D;
		createInfo.components.r = vk::ComponentSwizzle::eIdentity;
		createInfo.components.g = vk::ComponentSwizzle::eIdentity;
		createInfo.components.b = vk::ComponentSwizzle::eIdentity;
		createInfo.components.a = vk::ComponentSwizzle::eIdentity;
		createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		createInfo.format = format.format;

		bundle.frames[i].image = images[i];
		bundle.frames[i].imageView = logicalDevice.createImageView(createInfo);
	}

	bundle.format = format.format;
	bundle.extent = extent;

	return bundle;
}
