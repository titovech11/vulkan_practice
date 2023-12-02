#include "sync.h"

vk::Semaphore vkInit::make_semaphore(const vk::Device& device, const bool debug) {

	vk::SemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.flags = vk::SemaphoreCreateFlags();
	
	try {
		return device.createSemaphore(semaphoreInfo);
	}
	catch (vk::SystemError err) {
		
		std::cout << "Failed to create semaphore - " << err.what() << std::endl;
		
		return nullptr;
	}

}

vk::Fence vkInit::make_fence(const vk::Device& device, const bool debug) {

	vk::FenceCreateInfo fenceInfo = {};
	fenceInfo.flags = vk::FenceCreateFlags() | vk::FenceCreateFlagBits::eSignaled;

	try {

		return device.createFence(fenceInfo);
	}
	catch (vk::SystemError err) {

		std::cout << "Failed to create fence - " << err.what() << std::endl;

		return nullptr;
	}
}
