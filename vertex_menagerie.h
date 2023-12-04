#pragma once
#include "config.h"
#include "memory.h"

class VertexMenagerie {
public:
	VertexMenagerie();
	~VertexMenagerie();

	void consume(meshTypes type, const std::vector<float>& vertexData);
	void finalize(const vk::Device& logicalDevice, const vk::PhysicalDevice& physicalDevice);
	Buffer vertexBuffer_;
	std::unordered_map<meshTypes, int> offsets;
	std::unordered_map<meshTypes, int> sizes;
private:
	int offset;
	vk::Device logicalDevice_;
	std::vector<float> lump;
};