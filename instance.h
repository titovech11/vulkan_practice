#pragma once
#include "config.h"

namespace vkInit {

	bool supported(std::vector<const char*>& extensions, std::vector<const char*>& layers, const bool debug);

	vk::Instance make_instance(const bool debug, const char* appName);
}