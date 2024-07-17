#pragma once
#include "config.h"
#include "render_structs.h"

namespace vkInit {

	struct GraphicsPipelineInBundle {
		vk::Device device;
		std::string vertexFilepath;
		std::string fragmentFilepath;
		vk::Extent2D swapchainExtent;
		vk::Format swapchainImageFormat;
		vk::DescriptorSetLayout descriptorSetLayout;
	};

	struct GraphicsPipelineOutBundle {
		vk::PipelineLayout layout;
		vk::RenderPass renderpass;
		vk::Pipeline pipeline;
	};

	vk::PipelineLayout make_pipeline_layout(const vk::Device& device, const vk::DescriptorSetLayout& layout, const bool debug);

	vk::RenderPass make_renderpass(const vk::Device& device, const vk::Format& swapchainImageFormat, const bool debug);

	GraphicsPipelineOutBundle create_graphics_pipeline(const GraphicsPipelineInBundle& specification, const bool debug);
}