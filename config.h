#pragma once
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <optional>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <unordered_map>


struct BufferInput {
	size_t size;
	vk::BufferUsageFlags usage;
	vk::Device logicalDevice;
	vk::PhysicalDevice physicalDevice;
};

struct Buffer {
	vk::Buffer buffer;
	vk::DeviceMemory bufferMemory;
};

enum class meshTypes {
	TRIANGLE,
	SQUARE,
	STAR
};