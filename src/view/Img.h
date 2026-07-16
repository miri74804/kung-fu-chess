#pragma once

#include <opencv2/opencv.hpp>
#include <string>

// Thin wrapper around cv::Mat. This is the ONLY graphics primitive allowed
// in this project — all drawing (board, pieces, animations, score) goes
// through this class, per course rules.
class Img {
public:
	Img();

	// Loads an image from disk, optionally resizing it.
	// size = {0,0} means "keep original size".
	Img& read(const std::string& path,
		const std::pair<int, int>& size = {},
		bool keep_aspect = false,
		int interpolation = cv::INTER_AREA);

	// Draws this image onto other_img at pixel (x, y), alpha-blending if this image has a BGRA channel.
	void draw_on(Img& other_img, int x, int y);

	void put_text(const std::string& txt, int x, int y, double font_size,
		const cv::Scalar& color = cv::Scalar(255, 255, 255, 255),
		int thickness = 1);

	void show();

	// For a live game loop: shows this frame, waits at most waitMs for a key
	// (0 = keep going even with no key pressed), and - unlike show() - does
	// NOT destroy the window, so the same window keeps being reused frame
	// after frame. Returns the key code pressed (or -1 if none), so the
	// caller can check for e.g. ESC to exit the loop.
	int show(int waitMs);

	const cv::Mat& get_mat() const { return img; }
	bool is_loaded() const { return !img.empty(); }

private:
	cv::Mat img;
};
