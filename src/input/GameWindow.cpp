#include "GameWindow.h"
#include <opencv2/opencv.hpp>
#include <string>

namespace {
	// Must match the window title Img::show()/show(waitMs) draws into,
	// since setMouseCallback attaches to a window by name.
	const std::string WINDOW_NAME = "Image";
}

GameWindow::GameWindow() : clickPending(false), clickX(0), clickY(0) {
	// WINDOW_NORMAL, not AUTOSIZE: CTD.cpp calls enableFullscreen() right
	// after constructing this window, and OpenCV's WND_PROP_FULLSCREEN (at
	// least on the Win32 backend) only actually takes effect on a
	// WINDOW_NORMAL window - on AUTOSIZE it silently no-ops, leaving the
	// regular title bar in place (visible proof: a manually letterboxed,
	// screen-sized frame still showed a black gap under the title bar,
	// since the window itself wasn't actually screen-sized). This isn't
	// the same non-uniform-stretch risk WINDOW_NORMAL caused earlier: once
	// truly fullscreen there's no border left for the user to drag, so the
	// window can't drift to some other aspect ratio underneath us the way
	// a normal resizable window could.
	cv::namedWindow(WINDOW_NAME, cv::WINDOW_NORMAL);
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

void GameWindow::enableFullscreen() {
	cv::setWindowProperty(WINDOW_NAME, cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
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
