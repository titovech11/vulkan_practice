#pragma once
#include "config.h"

namespace vkUtil {

	Buffer createBuffer(const BufferInput& input);

	uint32_t findMemoryTypeIndex(vk::PhysicalDevice physicalDevice, uint32_t supportedMemoryIndices, vk::MemoryPropertyFlags requestedProperties);

	void allocateBufferMemory(Buffer& buffer, const BufferInput& input);

	void copyBuffer(const Buffer& srcBuffer, Buffer& dstBuffer, vk::DeviceSize size, vk::Queue& queue, vk::CommandBuffer& commandBuffer);
}