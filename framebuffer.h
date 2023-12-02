#pragma once
#include "config.h"
#include "frame.h"

namespace vkInit {

	struct framebufferInput {
		vk::Device device;
		vk::RenderPass renderpass;
		vk::Extent2D swapchainExtent;
	};

	void make_framebuffers(const framebufferInput& inputChunk, std::vector<vkUtil::SwapChainFrame>& frames, const bool debug);
}