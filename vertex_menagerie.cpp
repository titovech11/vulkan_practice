#include "vertex_menagerie.h"

VertexMenagerie::VertexMenagerie() {
	offset = 0;
}

VertexMenagerie::~VertexMenagerie(){

	logicalDevice_.destroyBuffer(vertexBuffer_.buffer);
	logicalDevice_.freeMemory(vertexBuffer_.bufferMemory);
}

void VertexMenagerie::consume(meshTypes type, const std::vector<float>& vertexData){

	for (float attribute : vertexData) {
		lump.push_back(attribute);
	}
	int vertexCount = static_cast<int>(vertexData.size() / 5);

	offsets.insert(std::make_pair(type, offset));
	sizes.insert(std::make_pair(type, vertexCount));

	offset += vertexCount;
}

void VertexMenagerie::finalize(FinalizationChunk finalizationChunk){

	logicalDevice_ = finalizationChunk.logicalDevice;
	BufferInput inputChunk;
	inputChunk.logicalDevice = finalizationChunk.logicalDevice;
	inputChunk.physicalDevice = finalizationChunk.physicalDevice;
	inputChunk.size = sizeof(float) * lump.size();

	inputChunk.usage = vk::BufferUsageFlagBits::eTransferSrc;
	inputChunk.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
	Buffer stagingBuffer = vkUtil::createBuffer(inputChunk);

	void* memoryLocation = finalizationChunk.logicalDevice.mapMemory(stagingBuffer.bufferMemory, 0, inputChunk.size);
	memcpy(memoryLocation, lump.data(), inputChunk.size);
	finalizationChunk.logicalDevice.unmapMemory(stagingBuffer.bufferMemory);

	inputChunk.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
	inputChunk.memoryProperties =vk::MemoryPropertyFlagBits::eDeviceLocal;
	vertexBuffer_ = vkUtil::createBuffer(inputChunk);

	vkUtil::copyBuffer(stagingBuffer, vertexBuffer_, inputChunk.size, finalizationChunk.queue, finalizationChunk.commandBuffer);

	logicalDevice_.destroyBuffer(stagingBuffer.buffer);
	logicalDevice_.freeMemory(stagingBuffer.bufferMemory);
}
