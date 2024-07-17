#include "scene.h"
#include <chrono>
#include <ctime>


Scene::Scene() {

	float x = -0.6f;
	for (float y = -0.8f; y < 1.0f; y += 0.6f) {

		trianglePositions.push_back(glm::vec3(x, y, 0.0f));

	}

	x = 0.0f;
	for (float y = -0.6f; y < 1.0f; y += 0.3f) {

		squarePositions.push_back(glm::vec3(x, y, 0.0f));

	}

	x = 0.6f;
	for (float y = -0.75f; y < 1.0f; y += 0.2f) {

		starPositions.push_back(glm::vec3(x, y, 0.0f));

	}
	//trianglePositions.push_back(glm::vec3(0, 0, 0.0f));
}