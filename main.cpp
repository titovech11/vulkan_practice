#include "app.h"

int main() {

	App* curApp = new App(640, 480, "vulkan wnd", true);

	curApp->run();
	delete curApp;

	return 0;
}