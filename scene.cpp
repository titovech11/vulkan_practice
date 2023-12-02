#include "scene.h"
#include <chrono>
#include <ctime>


Scene::Scene() {

    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

	for (float x = -1.0f; x < 1.0f; x += 0.4f) {
		for (float y = -1.0f; y < 1.0f; y += 0.4f) {
			trianglePositions.push_back(glm::vec3(x, y, 0.0f));
		}
	}
}