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

	// Switches to borderless fullscreen. Safe to combine with AUTOSIZE:
	// AUTOSIZE just means "don't let the user manually resize this window",
	// it doesn't stop the OS fullscreen property from resizing it to the
	// screen's resolution - so whatever image is shown after this call
	// still gets stretched to fill the screen exactly like before. The
	// caller is responsible for pre-scaling frames to the real screen size
	// itself (see CTD.cpp) so that stretch is a no-op instead of a distortion.
	void enableFullscreen();

private:
	bool clickPending;
	int clickX;
	int clickY;

	static void onMouse(int event, int x, int y, int flags, void* userdata);
};
