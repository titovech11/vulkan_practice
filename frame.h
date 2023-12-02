#pragma once
#include "config.h"

namespace vkUtil {

	struct SwapChainFrame {
		vk::Image image;
		vk::ImageView imageView;
		vk::Framebuffer frameBuffer;
		vk::CommandBuffer commandBuffer;
		vk::Semaphore imageAvailable, renderFinished;
		vk::Fence inFlight;
	};
}
