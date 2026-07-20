#pragma once

// Owns the OS-level game window's mouse capture. Not part of Img: drawing
// still goes exclusively through Img::show(waitMs); this class only asks
// the window "was there a click since I last checked?" via OpenCV's
// setMouseCallback - the "OS service that tells you about clicks."
class GameWindow {
public:
	GameWindow();
	~GameWindow();

	// Returns true and fills x,y exactly once per left-click since the last
	// call (edge-triggered), false otherwise.
	bool pollClick(int& x, int& y);

	// Switches to borderless fullscreen (see GameWindow.cpp's constructor
	// for why this needs WINDOW_NORMAL). The OS then stretches whatever
	// image is shown to the screen's resolution - CTD.cpp is responsible
	// for pre-scaling frames to that same real screen size, so that
	// stretch is a no-op instead of a distortion.
	void enableFullscreen();

private:
	bool clickPending;
	int clickX;
	int clickY;

	static void onMouse(int event, int x, int y, int flags, void* userdata);
};
