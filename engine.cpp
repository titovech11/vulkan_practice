#include "engine.h"
#include "instance.h"
#include "logging.h"
#include "device.h"
#include "swapchain.h"
#include "pipeline.h"
#include "framebuffer.h"
#include "commands.h"
#include "sync.h"


Engine::Engine(int width, int height, GLFWwindow* window, bool debug) {

	width_ = width;
	height_ = height;
	window_ = window;
	debugMode = debug;

	if (debugMode) {
		std::cout << "Making a graphics engine\n";
	}

	make_instance();

	make_device();

	make_pipeline();

	finalize_setup();

	make_assets();
}

void Engine::make_instance()
{
	instance = vkInit::make_instance(debugMode, "vulkan wnd");
	dldi = vk::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr);

	if (debugMode) {
		debugMessenger = vkInit::make_debug_messenger(instance, dldi);
	}
	VkSurfaceKHR c_style_surface;
	
	if (glfwCreateWindowSurface(instance, window_, nullptr, &c_style_surface) != VK_SUCCESS) {
		if (debugMode) {
			std::cout << "Failed to abstract glfw surface for vulcan \n";
		}
	}
	else if (debugMode) {
		std::cout << "Successfully abstract glfw surface for vulcan \n";
	}
	surface = c_style_surface;
}

void Engine::make_device()
{
	physicalDevice = vkInit::choose_physical_device(instance, debugMode);
	device = vkInit::create_logical_device(physicalDevice, surface, debugMode);
	std::array<vk::Queue, 2> queues = vkInit::get_queues(physicalDevice, device, surface, debugMode);
	graphicsQueue = queues[0];
	presentQueue = queues[1];
	make_swapchain();
	frameNumber = 0;
}

void Engine::make_swapchain() {

	vkInit::SwapChainBundle bundle = vkInit::create_swapchain(device, physicalDevice, surface, width_, height_, debugMode);
	swapchain = bundle.swapchain;
	swapchainFrames = bundle.frames;
	swapchainExtent = bundle.extent;
	swapchainFormat = bundle.format;

	maxFramesInFLight = static_cast<int>(swapchainFrames.size());
}

void Engine::recreate_swapchain() {

	width_ = 0;
	height_ = 0;

	while (width_ == 0 || height_ == 0) {
		glfwGetFramebufferSize(window_, &width_, &height_);
		glfwWaitEvents();
	}

	device.waitIdle();

	cleanup_swapchain();
	make_swapchain();
	make_framebuffers();
	make_frame_sync_objects();

	vkInit::commandBufferInputChunk commandBufferInput = { device, commandPool, swapchainFrames };
	vkInit::make_frame_command_buffers(commandBufferInput, debugMode);
}

void Engine::make_framebuffers() {

	vkInit::framebufferInput frameBufferInput;
	frameBufferInput.device = device;
	frameBufferInput.renderpass = renderpass;
	frameBufferInput.swapchainExtent = swapchainExtent;
	vkInit::make_framebuffers(frameBufferInput, swapchainFrames, debugMode);
}

void Engine::make_pipeline() {

	vkInit::GraphicsPipelineInBundle specification = {};
	specification.device = device;
	specification.vertexFilepath = "shaders/vertex.spv";
	specification.fragmentFilepath = "shaders/fragment.spv";
	specification.swapchainExtent = swapchainExtent;
	specification.swapchainImageFormat = swapchainFormat;

	vkInit::GraphicsPipelineOutBundle output = vkInit::create_graphics_pipeline(
		specification, debugMode
	);

	pipelineLayout = output.layout;
	renderpass = output.renderpass;
	pipeline = output.pipeline;
}

void Engine::finalize_setup() {

	make_framebuffers();

	commandPool = vkInit::make_command_pool(device, physicalDevice, surface, debugMode);

	vkInit::commandBufferInputChunk commandBufferInput = { device, commandPool, swapchainFrames };
	mainCommandBuffer = vkInit::make_command_buffer(commandBufferInput, debugMode);
	vkInit::make_frame_command_buffers(commandBufferInput, debugMode);
	
	make_frame_sync_objects();
}

void Engine::make_frame_sync_objects() {

	for (vkUtil::SwapChainFrame& frame : swapchainFrames) {
		frame.inFlight = vkInit::make_fence(device, debugMode);
		frame.imageAvailable = vkInit::make_semaphore(device, debugMode);
		frame.renderFinished = vkInit::make_semaphore(device, debugMode);
	}
}

void Engine::record_draw_commands(const vk::CommandBuffer& commandBuffer, uint32_t imageIndex, Scene* scene) {

	vk::CommandBufferBeginInfo beginInfo = {};
	try {
		commandBuffer.begin(beginInfo);
	}
	catch (vk::SystemError err) {
		std::cout << "Failed to begin recording command buffer - " << err.what() << std::endl;
	}

	vk::RenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.renderPass = renderpass;
	renderPassInfo.framebuffer = swapchainFrames[imageIndex].frameBuffer;
	renderPassInfo.renderArea.offset.x = 0;
	renderPassInfo.renderArea.offset.y = 0;
	renderPassInfo.renderArea.extent = swapchainExtent;

	vk::ClearValue clearColor = { std::array<float, 4>{1.0f, 1.0f, 0.25f, 1.0f} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	commandBuffer.beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);

	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

	prepare_scene(commandBuffer);
	//int firstVertex = meshes->offsets.find(meshTypes::TRIANGLE)->second;
	//int vertexCount = meshes->sizes.find(meshTypes::TRIANGLE)->second;

	//for (glm::vec3 position : scene->trianglePositions) {
	//	glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
	//	vkUtil::ObjectData objectData;
	//	objectData.model = model;
	//	commandBuffer.pushConstants(pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(objectData), &objectData);
	//	commandBuffer.draw(vertexCount, 1, firstVertex, 0);
	//}
	
	int vertexCount = meshes->sizes.find(meshTypes::TRIANGLE)->second;
	int firstVertex = meshes->offsets.find(meshTypes::TRIANGLE)->second;
	for (glm::vec3 position : scene->trianglePositions) {

		glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
		vkUtil::ObjectData objectData;
		objectData.model = model;
		commandBuffer.pushConstants(
			pipelineLayout, vk::ShaderStageFlagBits::eVertex,
			0, sizeof(objectData), &objectData
		);

		commandBuffer.draw(vertexCount, 1, firstVertex, 0);

	}

	vertexCount = meshes->sizes.find(meshTypes::SQUARE)->second;
	firstVertex = meshes->offsets.find(meshTypes::SQUARE)->second;
	for (glm::vec3 position : scene->squarePositions) {

		glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
		vkUtil::ObjectData objectData;
		objectData.model = model;
		commandBuffer.pushConstants(
			pipelineLayout, vk::ShaderStageFlagBits::eVertex,
			0, sizeof(objectData), &objectData
		);

		commandBuffer.draw(vertexCount, 1, firstVertex, 0);

	}

	vertexCount = meshes->sizes.find(meshTypes::STAR)->second;
	firstVertex = meshes->offsets.find(meshTypes::STAR)->second;
	for (glm::vec3 position : scene->starPositions) {

		glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
		vkUtil::ObjectData objectData;
		objectData.model = model;
		commandBuffer.pushConstants(
			pipelineLayout, vk::ShaderStageFlagBits::eVertex,
			0, sizeof(objectData), &objectData
		);

		commandBuffer.draw(vertexCount, 1, firstVertex, 0);

	}

	commandBuffer.endRenderPass();

	try {
		commandBuffer.end();
	}
	catch (vk::SystemError err) {
		std::cout << "Failed to record command buffer - " << err.what() << std::endl;
	}
}

void Engine::make_assets() {

	meshes = new VertexMenagerie();

	meshTypes type = meshTypes::TRIANGLE;
	std::vector<float> vertices = { {
		 0.0f, -0.15f, 1.0f, 0.0f, 1.0f,
		 0.15f, 0.15f, 0.0f, 1.0f, 0.0f,
		-0.15f, 0.15f, 0.0f, 1.0f, 0.0f
	} };
	meshes->consume(type, vertices);

	type = meshTypes::SQUARE;
	vertices = { {
		-0.05f,  0.05f, 0.0f, 1.0f, 1.0f,
		-0.05f, -0.05f, 1.0f, 0.0f, 0.0f,
		 0.05f, -0.05f, 1.0f, 0.0f, 0.0f,
		 0.05f, -0.05f, 1.0f, 0.0f, 0.0f,
		 0.05f,  0.05f, 1.0f, 0.0f, 0.0f,
		-0.05f,  0.05f, 0.0f, 1.0f, 1.0f
	} };
	meshes->consume(type, vertices);

	type = meshTypes::STAR;
	vertices = { {
		-0.05f, -0.025f, 1.0f, 0.0f, 1.0f,
		-0.02f, -0.025f, 0.0f, 0.0f, 1.0f,
		-0.03f,    0.0f, 0.0f, 0.0f, 1.0f,
		-0.02f, -0.025f, 1.0f, 0.0f, 1.0f,
		  0.0f,  -0.05f, 0.0f, 0.0f, 1.0f,
		 0.02f, -0.025f, 0.0f, 0.0f, 1.0f,
		-0.03f,    0.0f, 1.0f, 0.0f, 1.0f,
		-0.02f, -0.025f, 0.0f, 0.0f, 1.0f,
		 0.02f, -0.025f, 0.0f, 0.0f, 1.0f,
		 0.02f, -0.025f, 0.0f, 0.0f, 1.0f,
		 0.05f, -0.025f, 1.0f, 0.0f, 1.0f,
		 0.03f,    0.0f, 0.0f, 0.0f, 1.0f,
		-0.03f,    0.0f, 0.0f, 0.0f, 1.0f,
		 0.02f, -0.025f, 1.0f, 0.0f, 1.0f,
		 0.03f,    0.0f, 0.0f, 0.0f, 1.0f,
		 0.03f,    0.0f, 0.0f, 0.0f, 1.0f,
		 0.04f,   0.05f, 0.0f, 0.0f, 1.0f,
		  0.0f,   0.01f, 1.0f, 0.0f, 1.0f,
		-0.03f,    0.0f, 0.0f, 1.0f, 1.0f,
		 0.03f,    0.0f, 0.0f, 0.0f, 1.0f,
		  0.0f,   0.01f, 1.0f, 0.0f, 1.0f,
		-0.03f,    0.0f, 0.0f, 0.0f, 1.0f,
		  0.0f,   0.01f, 1.0f, 0.0f, 1.0f,
		-0.04f,   0.05f, 0.0f, 0.0f, 1.0f
	} };
	meshes->consume(type, vertices);

	meshes->finalize(device, physicalDevice);
}

void Engine::prepare_scene(const vk::CommandBuffer& commandBuffer) {

	vk::Buffer vertexBuffers[] = { meshes->vertexBuffer_.buffer };
	vk::DeviceSize offsets[] = { 0 };
	commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
}

void Engine::render(Scene* scene) {

	device.waitForFences(1, &swapchainFrames[frameNumber].inFlight, VK_TRUE, UINT64_MAX);

	//acquireNextImageKHR(vk::SwapChainKHR, timeout, semaphore_to_signal, fence)
	uint32_t imageIndex;
	try {
		vk::ResultValue acquire = device.acquireNextImageKHR(swapchain, UINT64_MAX, swapchainFrames[frameNumber].imageAvailable, nullptr);
		imageIndex = acquire.value;
	}
	catch (vk::OutOfDateKHRError err) {
		recreate_swapchain();
		return;
	}

	vk::CommandBuffer commandBuffer = swapchainFrames[frameNumber].commandBuffer;

	commandBuffer.reset();

	record_draw_commands(commandBuffer, imageIndex, scene);

	vk::SubmitInfo submitInfo = {};

	vk::Semaphore waitSemaphores[] = { swapchainFrames[frameNumber].imageAvailable };
	vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vk::Semaphore signalSemaphores[] = { swapchainFrames[frameNumber].renderFinished };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	device.resetFences(1, &swapchainFrames[frameNumber].inFlight);
	try {
		graphicsQueue.submit(submitInfo, swapchainFrames[frameNumber].inFlight);
	}
	catch (vk::SystemError err) {

		std::cout << "Failed to submit draw command buffer - " << err.what() << std::endl;
	}

	vk::PresentInfoKHR presentInfo = {};
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	vk::SwapchainKHR swapChains[] = { swapchain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	vk::Result  present;

	try {
		present = presentQueue.presentKHR(presentInfo);
	}
	catch(vk::OutOfDateKHRError err) {
		present = vk::Result::eErrorOutOfDateKHR;
	}

	if (present == vk::Result::eErrorOutOfDateKHR || present == vk::Result::eSuboptimalKHR) {
		recreate_swapchain();
		return;
	}

	frameNumber = (frameNumber + 1) % maxFramesInFLight;
}

void Engine::cleanup_swapchain() {

	for (vkUtil::SwapChainFrame& frame : swapchainFrames) {
		device.destroyImageView(frame.imageView);
		device.destroyFramebuffer(frame.frameBuffer);
		device.destroyFence(frame.inFlight);
		device.destroySemaphore(frame.imageAvailable);
		device.destroySemaphore(frame.renderFinished);
	}

	device.destroySwapchainKHR(swapchain);
}

Engine::~Engine() {

	device.waitIdle();

	if (debugMode) {
		std::cout << "engine dtor\n";
	}

	device.destroyCommandPool(commandPool);

	device.destroyPipeline(pipeline);
	device.destroyPipelineLayout(pipelineLayout);
	device.destroyRenderPass(renderpass);

	cleanup_swapchain();

	delete meshes;

	device.destroy();

	instance.destroySurfaceKHR(surface);
	if (debugMode) {
		instance.destroyDebugUtilsMessengerEXT(debugMessenger, nullptr, dldi);
	}
	instance.destroy();

	glfwTerminate();
}