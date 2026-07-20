#include "GameWindow.h"
#include "../Constants.h"
#include <opencv2/opencv.hpp>

GameWindow::GameWindow() : clickPending(false), clickX(0), clickY(0) {
	// WINDOW_NORMAL, not AUTOSIZE: enableFullscreen() gets called right
	// after this constructor, and OpenCV's WND_PROP_FULLSCREEN (at least on
	// the Win32 backend) silently no-ops on an AUTOSIZE window. This isn't
	// the non-uniform-stretch risk WINDOW_NORMAL caused before we went
	// fullscreen: with no border left for the user to drag, the window
	// can't drift to some other aspect ratio underneath us.
	cv::namedWindow(GAME_WINDOW_NAME, cv::WINDOW_NORMAL);
	cv::setMouseCallback(GAME_WINDOW_NAME, onMouse, this);
}

GameWindow::~GameWindow() {
	cv::destroyWindow(GAME_WINDOW_NAME);
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
	cv::setWindowProperty(GAME_WINDOW_NAME, cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
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
