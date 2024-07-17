#include "frame.h"
#include "memory.h"

void vkUtil::SwapChainFrame::make_descriptor_resources(const vk::Device& logicalDevice, const vk::PhysicalDevice& physicalDevice) {

	BufferInput input;
	input.logicalDevice = logicalDevice;
	input.physicalDevice = physicalDevice;
	input.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
	input.size = sizeof(UniformBufferObj);
	input.usage = vk::BufferUsageFlagBits::eUniformBuffer;
	cameraDataBuffer = createBuffer(input);

	cameraDataWriteLOcation = logicalDevice.mapMemory(cameraDataBuffer.bufferMemory, 0, sizeof(UniformBufferObj));

    input.size = 1024 * sizeof(glm::mat4);
    input.usage = vk::BufferUsageFlagBits::eStorageBuffer;
    modelBuffer = createBuffer(input);

    modelBufferWriteLocation = logicalDevice.mapMemory(modelBuffer.bufferMemory, 0, 1024 * sizeof(glm::mat4));

    modelTransforms.reserve(1024);
    for (int i = 0; i < 1024; ++i) {
        modelTransforms.push_back(glm::mat4(1.0f));
    }

	uniformBufferDescriptor.buffer = cameraDataBuffer.buffer;
	uniformBufferDescriptor.offset = 0;
	uniformBufferDescriptor.range = sizeof(UniformBufferObj);

    modelBufferDescriptor.buffer = modelBuffer.buffer;
    modelBufferDescriptor.offset = 0;
    modelBufferDescriptor.range = 1024 * sizeof(glm::mat4);

}

void vkUtil::SwapChainFrame::write_descriptor_set(const vk::Device& logicalDevice) {

	vk::WriteDescriptorSet writeInfoUniform;
    writeInfoUniform.dstSet = descriptorSet;
    writeInfoUniform.dstBinding = 0;
    writeInfoUniform.dstArrayElement = 0;
    writeInfoUniform.descriptorCount = 1;
    writeInfoUniform.descriptorType = vk::DescriptorType::eUniformBuffer;
    writeInfoUniform.pBufferInfo = &uniformBufferDescriptor;

	logicalDevice.updateDescriptorSets(writeInfoUniform, nullptr);

    vk::WriteDescriptorSet writeInfoStorage;
    writeInfoStorage.dstSet = descriptorSet;
    writeInfoStorage.dstBinding = 1;
    writeInfoStorage.dstArrayElement = 0;
    writeInfoStorage.descriptorCount = 1;
    writeInfoStorage.descriptorType = vk::DescriptorType::eStorageBuffer;
    writeInfoStorage.pBufferInfo = &modelBufferDescriptor;

    logicalDevice.updateDescriptorSets(writeInfoStorage, nullptr);
}
