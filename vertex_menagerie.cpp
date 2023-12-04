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

void VertexMenagerie::finalize(const vk::Device& logicalDevice, const vk::PhysicalDevice& physicalDevice){

	logicalDevice_ = logicalDevice;

	BufferInput inputChunk;
	inputChunk.logicalDevice = logicalDevice;
	inputChunk.physicalDevice = physicalDevice;
	inputChunk.size = sizeof(float) * lump.size();
	inputChunk.usage = vk::BufferUsageFlagBits::eVertexBuffer;

	vertexBuffer_ = vkUtil::createBuffer(inputChunk);

	void* memoryLocation = logicalDevice.mapMemory(vertexBuffer_.bufferMemory, 0, inputChunk.size);
	memcpy(memoryLocation, lump.data(), inputChunk.size);
	logicalDevice.unmapMemory(vertexBuffer_.bufferMemory);
}
