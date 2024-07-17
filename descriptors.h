#pragma once
#include "config.h"

namespace vkInit {

	struct descriptorSetLayoutData {
		int count;
		std::vector<int> indices;
		std::vector<vk::DescriptorType> types;
		std::vector<int> counts;
		std::vector<vk::ShaderStageFlags> stages;
	};

	vk::DescriptorSetLayout make_descriptor_set_layout(const vk::Device& logicalDevice, const descriptorSetLayoutData& bindings);

	vk::DescriptorPool make_descriptor_pool(const vk::Device& logicalDevice, uint32_t size, const descriptorSetLayoutData& bindings);

	vk::DescriptorSet allocate_descriptor_set(const vk::Device& logicalDevice, const vk::DescriptorPool& descriptorPool, const vk::DescriptorSetLayout& layout);
}