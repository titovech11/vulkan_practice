#pragma once

#include "config.h"

namespace vkMesh {

	vk::VertexInputBindingDescription getPosColorBindingDescription();

	std::array<vk::VertexInputAttributeDescription, 2> getPosColorAttributeDescription();
}