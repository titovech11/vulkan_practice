#pragma once
#include "config.h"

namespace vkUtil {

	struct UniformBufferObj {
		glm::mat4 view;
		glm::mat4 projection;
		glm::mat4 viewProjection;
	};

	struct SwapChainFrame {
		vk::Image image;
		vk::ImageView imageView;
		vk::Framebuffer frameBuffer;

		vk::CommandBuffer commandBuffer;

		vk::Semaphore imageAvailable, renderFinished;
		vk::Fence inFlight;

		UniformBufferObj cameraData;
		Buffer cameraDataBuffer;
		void* cameraDataWriteLOcation;

        std::vector<glm::mat4> modelTransforms;
        Buffer modelBuffer;
        void* modelBufferWriteLocation;

		vk::DescriptorBufferInfo uniformBufferDescriptor;
        vk::DescriptorBufferInfo modelBufferDescriptor;
		vk::DescriptorSet descriptorSet;

		void make_descriptor_resources(const vk::Device& logicalDevice, const vk::PhysicalDevice& physicalDevice);
		void write_descriptor_set(const vk::Device& logicalDevice);
	};
}
