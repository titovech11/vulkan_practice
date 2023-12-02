#pragma once
#include "config.h"
#include "queue_families.h"
#include "frame.h"


namespace vkInit {

	struct commandBufferInputChunk {
		vk::Device device;
		vk::CommandPool commandPool;
		std::vector<vkUtil::SwapChainFrame>& frames;
	};

	vk::CommandPool make_command_pool(const vk::Device& device, const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface, const bool debug);

	vk::CommandBuffer make_command_buffer(commandBufferInputChunk& inputChunk, const bool debug);

	void make_frame_command_buffers(commandBufferInputChunk inputChunk, bool debug);

}