#include "framebuffer.h"

void vkInit::make_framebuffers(const framebufferInput& inputChunk, std::vector<vkUtil::SwapChainFrame>& frames, const bool debug) {

	for (int i = 0; i != frames.size(); ++i) {

		std::vector<vk::ImageView> attachments = { frames[i].imageView };

		vk::FramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.flags = vk::FramebufferCreateFlags();
		framebufferInfo.renderPass = inputChunk.renderpass;
		framebufferInfo.attachmentCount = attachments.size();
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = inputChunk.swapchainExtent.width;
		framebufferInfo.height = inputChunk.swapchainExtent.height;
		framebufferInfo.layers = 1;

		try {
			frames[i].frameBuffer = inputChunk.device.createFramebuffer(framebufferInfo);

			if (debug) {
				std::cout << "Created framebuffer fo frame " << i << std::endl;
			}
		}
		catch (vk::SystemError err) {
			std::cout << "Failed to create framebuffer for frame " << i << " - " << err.what() << std::endl;
		}
	}
}
