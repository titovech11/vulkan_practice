#include "memory.h"

Buffer vkUtil::createBuffer(const BufferInput& input) {

    Buffer buffer;
    vk::BufferCreateInfo bufferInfo;
    bufferInfo.flags = vk::BufferCreateFlags();
    bufferInfo.size = input.size;
    bufferInfo.usage = input.usage;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;

    buffer.buffer = input.logicalDevice.createBuffer(bufferInfo);

    allocateBufferMemory(buffer, input);

    return buffer;
}

uint32_t vkUtil::findMemoryTypeIndex(vk::PhysicalDevice physicalDevice, uint32_t supportedMemoryIndices, vk::MemoryPropertyFlags requestedProperties) {

    vk::PhysicalDeviceMemoryProperties memoryProperties = physicalDevice.getMemoryProperties();

    for (uint32_t i = 0; i != memoryProperties.memoryTypeCount; ++i) {

        bool supported{ static_cast<bool>(supportedMemoryIndices & (1 << i)) };

        bool sufficient{ (memoryProperties.memoryTypes[i].propertyFlags & requestedProperties) == requestedProperties };

        if (supported && sufficient) {
            return i;
        }
    }

    return 0;
}

void vkUtil::allocateBufferMemory(Buffer& buffer, const BufferInput& input) {

    vk::MemoryRequirements memoryRequirements = input.logicalDevice.getBufferMemoryRequirements(buffer.buffer);

    vk::MemoryAllocateInfo allocInfo;
    allocInfo.allocationSize = memoryRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryTypeIndex(input.physicalDevice, memoryRequirements.memoryTypeBits,
    input.memoryProperties);

    buffer.bufferMemory = input.logicalDevice.allocateMemory(allocInfo);
    input.logicalDevice.bindBufferMemory(buffer.buffer, buffer.bufferMemory, 0);
}

void vkUtil::copyBuffer(const Buffer& srcBuffer, Buffer& dstBuffer, vk::DeviceSize size, vk::Queue& queue, vk::CommandBuffer& commandBuffer) {

    commandBuffer.reset();

    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    commandBuffer.begin(beginInfo);

    vk::BufferCopy copyRegion;
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    commandBuffer.copyBuffer(srcBuffer.buffer, dstBuffer.buffer, 1, &copyRegion);

    commandBuffer.end();

    vk::SubmitInfo submitInfo;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    queue.submit(1, &submitInfo, nullptr);
    queue.waitIdle();
}
