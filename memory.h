#pragma once
#include "config.h"

namespace vkUtil {

	Buffer createBuffer(const BufferInput& input);

	uint32_t findMemoryTypeIndex(vk::PhysicalDevice physicalDevice, uint32_t supportedMemoryIndices, vk::MemoryPropertyFlags requestedProperties);

	void allocateBufferMemory(Buffer& buffer, const BufferInput& input);
}