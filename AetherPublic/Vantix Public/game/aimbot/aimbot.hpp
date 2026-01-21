#include "interception.h"
#include <thread>
InterceptionContext mouse_context;
InterceptionDevice mouse_device;
InterceptionStroke mouse_stroke;


void run_mouse_loop() {
	while (interception_receive(mouse_context, mouse_device = interception_wait(mouse_context), &mouse_stroke, 1) > 0) {
		if (interception_is_mouse(mouse_device)) {
			InterceptionMouseStroke& mstroke = *(InterceptionMouseStroke*)&mouse_stroke;
			interception_send(mouse_context, mouse_device, &mouse_stroke, 1);
		}
	}
}

bool initialize_mouse() {
	mouse_context = interception_create_context();
	interception_set_filter(mouse_context, interception_is_mouse, INTERCEPTION_FILTER_MOUSE_MOVE);
	mouse_device = interception_wait(mouse_context);

	while (interception_receive(mouse_context, mouse_device = interception_wait(mouse_context), &mouse_stroke, 1) > 0) {
		if (interception_is_mouse(mouse_device)) {
			InterceptionMouseStroke& mstroke = *(InterceptionMouseStroke*)&mouse_stroke;
			interception_send(mouse_context, mouse_device, &mouse_stroke, 1);
			break;
		}
	}

	std::thread([&]() { run_mouse_loop(); }).detach();

	return true;
}

//void move_mouse(int x, int y) {
//	InterceptionMouseStroke& mstroke = *(InterceptionMouseStroke*)&mouse_stroke;
//
//	mstroke.flags = 0;
//	mstroke.information = 0;
//	mstroke.x = x;
//	mstroke.y = y;
//
//	interception_send(mouse_context, mouse_device, &mouse_stroke, 1);
//}

void mouse_click() {

	InterceptionMouseStroke& mstroke = *(InterceptionMouseStroke*)&mouse_stroke;


	mstroke.state = INTERCEPTION_MOUSE_LEFT_BUTTON_DOWN;
	interception_send(mouse_context, mouse_device, &mouse_stroke, 1);

	std::this_thread::sleep_for(std::chrono::milliseconds(10));


	mstroke.state = INTERCEPTION_MOUSE_LEFT_BUTTON_UP;
	interception_send(mouse_context, mouse_device, &mouse_stroke, 1);
}
