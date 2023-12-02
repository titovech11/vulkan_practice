#include "commands.h"

vk::CommandPool vkInit::make_command_pool(const vk::Device& device, const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface, const bool debug) {

	vkUtil::QueueFamilyIndices queueFamilyIndeces = vkUtil::findQueueFamilies(physicalDevice, surface, debug);

	vk::CommandPoolCreateInfo poolInfo = {};
	poolInfo.flags = vk::CommandPoolCreateFlags() | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
	poolInfo.queueFamilyIndex = queueFamilyIndeces.graphicsFamily.value();

	try {
		return device.createCommandPool(poolInfo);
	}
	catch (vk::SystemError err) {

		std::cout << "Failed to create command pool - " << err.what() << std::endl;

		return nullptr;
	}
}

vk::CommandBuffer vkInit::make_command_buffer(commandBufferInputChunk& inputChunk, const bool debug) {

	vk::CommandBufferAllocateInfo allocInfo = {};
	allocInfo.commandPool = inputChunk.commandPool;
	allocInfo.level = vk::CommandBufferLevel::ePrimary;
	allocInfo.commandBufferCount = 1;

	//Make a "main" command buffer
	try {
		vk::CommandBuffer commandBuffer = inputChunk.device.allocateCommandBuffers(allocInfo)[0];

		if (debug) {
			std::cout << "Allocated main command buffer " << std::endl;
		}

		return commandBuffer;
	}
	catch (vk::SystemError err) {

		std::cout << "Failed to allocate main command buffer - " << err.what() << std::endl;

		return nullptr;
	}
}

void vkInit::make_frame_command_buffers(vkInit::commandBufferInputChunk inputChunk, bool debug) {

	vk::CommandBufferAllocateInfo allocInfo = {};
	allocInfo.commandPool = inputChunk.commandPool;
	allocInfo.level = vk::CommandBufferLevel::ePrimary;
	allocInfo.commandBufferCount = 1;

	//Make a command buffer for each frame
	for (int i = 0; i < inputChunk.frames.size(); ++i) {
		try {
			inputChunk.frames[i].commandBuffer = inputChunk.device.allocateCommandBuffers(allocInfo)[0];

			if (debug) {
				std::cout << "Allocated command buffer for frame " << i << std::endl;
			}
		}
		catch (vk::SystemError err) {

			std::cout << "Failed to allocate command buffer for frame " << i << " - " << err.what() << std::endl;
		}
	}
}