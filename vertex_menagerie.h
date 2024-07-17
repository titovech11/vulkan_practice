#pragma once
#include "config.h"
#include "memory.h"

struct FinalizationChunk {
	vk::Device logicalDevice;
	vk::PhysicalDevice physicalDevice;
	vk::Queue queue;
	vk::CommandBuffer commandBuffer;
};

class VertexMenagerie {
public:
	VertexMenagerie();
	~VertexMenagerie();

	void consume(meshTypes type, const std::vector<float>& vertexData);
	void finalize(FinalizationChunk finalizationChunk);
	Buffer vertexBuffer_;
	std::unordered_map<meshTypes, int> offsets;
	std::unordered_map<meshTypes, int> sizes;
private:
	int offset;
	vk::Device logicalDevice_;
	std::vector<float> lump;
};