#include "GameWindow.h"
#include <opencv2/opencv.hpp>

namespace {
	// Must match the window title Img::show()/show(waitMs) draws into,
	// since setMouseCallback attaches to a window by name.
	const char* WINDOW_NAME = "Image";
}

GameWindow::GameWindow() : clickPending(false), clickX(0), clickY(0) {
	cv::namedWindow(WINDOW_NAME, cv::WINDOW_AUTOSIZE);
	cv::setMouseCallback(WINDOW_NAME, onMouse, this);
}

GameWindow::~GameWindow() {
	cv::destroyWindow(WINDOW_NAME);
}

void GameWindow::onMouse(int event, int x, int y, int /*flags*/, void* userdata) {
	if (event != cv::EVENT_LBUTTONDOWN) {
		return;
	}
	GameWindow* self = static_cast<GameWindow*>(userdata);
	self->clickPending = true;
	self->clickX = x;
	self->clickY = y;
}

bool GameWindow::pollClick(int& x, int& y) {
	if (!clickPending) {
		return false;
	}
	clickPending = false;
	x = clickX;
	y = clickY;
	return true;
}
