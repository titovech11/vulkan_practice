#pragma once
#include "config.h"
#include "frame.h"
#include "scene.h"
#include "vertex_menagerie.h"

class Engine {

public:
	Engine(int width, int height, GLFWwindow* window, bool debug);

	~Engine();

	void render(Scene* scene);

private:

	bool debugMode = false;

	vk::Instance instance{ nullptr };
	vk::DebugUtilsMessengerEXT debugMessenger{ nullptr };
	vk::DispatchLoaderDynamic dldi;
	vk::SurfaceKHR surface;

	vk::PhysicalDevice physicalDevice{ nullptr };
	vk::Device device{ nullptr };
	vk::Queue graphicsQueue{ nullptr };
	vk::Queue presentQueue{ nullptr };
	vk::SwapchainKHR swapchain;
	std::vector<vkUtil::SwapChainFrame> swapchainFrames;
	vk::Format swapchainFormat;
	vk::Extent2D swapchainExtent;

	vk::PipelineLayout pipelineLayout;
	vk::RenderPass renderpass;
	vk::Pipeline pipeline;

	vk::CommandPool commandPool;
	vk::CommandBuffer mainCommandBuffer;

	int maxFramesInFLight, frameNumber;

	VertexMenagerie* meshes;

	int width_;
	int height_;
	GLFWwindow* window_;

	void make_instance();

	void make_device();
	void make_swapchain();
	void recreate_swapchain();

	void make_pipeline();

	void finalize_setup();
	void make_framebuffers();
	void make_frame_sync_objects();

	void make_assets();
	void prepare_scene(const vk::CommandBuffer& commandBuffer);

	void record_draw_commands(const vk::CommandBuffer& commandBuffer, uint32_t imageIndex, Scene* scene);

	void cleanup_swapchain();
};