#include "descriptors.h"

vk::DescriptorSetLayout vkInit::make_descriptor_set_layout(const vk::Device& logicalDevice, const descriptorSetLayoutData& bindings) {

    std::vector<vk::DescriptorSetLayoutBinding> layoutBindings;
    layoutBindings.reserve(bindings.count);

    for (int i = 0; i != bindings.count; ++i) {
        vk::DescriptorSetLayoutBinding layoutBinding;
        layoutBinding.binding = bindings.indices[i];
        layoutBinding.descriptorType = bindings.types[i];
        layoutBinding.descriptorCount = bindings.counts[i];
        layoutBinding.stageFlags = bindings.stages[i];
        layoutBindings.push_back(layoutBinding);
    }

    vk::DescriptorSetLayoutCreateInfo layoutInfo;

    layoutInfo.flags = vk::DescriptorSetLayoutCreateFlagBits();
    layoutInfo.bindingCount = bindings.count;
    layoutInfo.pBindings = layoutBindings.data();

    try {
        return logicalDevice.createDescriptorSetLayout(layoutInfo);
    }
    catch (vk::SystemError err) {
        std::cout << "Failed to create descriptor set layout - " << err.what() << std::endl;
        return nullptr;
    }
}

vk::DescriptorPool vkInit::make_descriptor_pool(const vk::Device& logicalDevice, uint32_t size, const descriptorSetLayoutData& bindings) {

    std::vector<vk::DescriptorPoolSize> poolSizes;

    for (int i = 0; i != bindings.count; ++i) {
        vk::DescriptorPoolSize poolSize;
        poolSize.type = bindings.types[i];
        poolSize.descriptorCount = size;
        poolSizes.push_back(poolSize);
    }

    vk::DescriptorPoolCreateInfo poolInfo;

    poolInfo.flags = vk::DescriptorPoolCreateFlags();
    poolInfo.maxSets = size;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();

    try {
        return logicalDevice.createDescriptorPool(poolInfo);
    }
    catch (vk::SystemError err) {
        std::cout << "Failed to create Descriptor Pool - " << err.what() << std::endl;
        return nullptr;
    }
}

vk::DescriptorSet vkInit::allocate_descriptor_set(const vk::Device& logicalDevice, const vk::DescriptorPool& descriptorPool, const vk::DescriptorSetLayout& layout) {
    
    vk::DescriptorSetAllocateInfo allocInfo;

    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;

    try {
        return logicalDevice.allocateDescriptorSets(allocInfo)[0];
    } 
    catch (vk::SystemError err) {
        std::cout << "Failed to allocate descriptor set from pool - " << err.what() << std::endl;
        return nullptr;
    }
}
