#pragma once
#include "config.h"

namespace vkInit {

	vk::Semaphore make_semaphore(const vk::Device& device, const bool debug);

	vk::Fence make_fence(const vk::Device& device, const bool debug);

}