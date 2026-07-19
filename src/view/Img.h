#pragma once

#include <opencv2/opencv.hpp>
#include <string>

// Thin wrapper around cv::Mat. This is the ONLY graphics primitive allowed
// in this project — all drawing (board, pieces, animations, score) goes
// through this class, per course rules.
class Img {
public:
	Img();

	// A solid-color canvas, not loaded from any file - for compositing
	// several pieces (board, side panels, ...) onto one wider image. Always
	// 4-channel BGRA (opaque, alpha=255 by default) to match board.png and
	// every piece sprite - see blank()'s definition for why that matters.
	static Img blank(int width, int height, const cv::Scalar& color = cv::Scalar(40, 40, 40, 255));

	// A real, independent pixel copy - NOT the same as `Img b = a;` (which
	// only copies cv::Mat's header and shares the same underlying pixels,
	// so drawing on the copy would silently also change the original).
	// Needed to cache an already-composited frame and safely draw on top
	// of a fresh copy of it each frame, without corrupting the cache.
	Img clone() const;

	// An independent copy of just the (x,y,w,h) sub-rectangle. Used to cut
	// a decorative border off a loaded image before resizing just the
	// interior to an exact target size - resizing the whole image at once
	// can't guarantee the interior lands on a whole number of pixels per
	// cell, which is what actually causes a highlight square (always
	// exactly CELL_SIZE) to drift off the board's own checkered squares.
	Img crop(int x, int y, int w, int h) const;

	// Resizes the already-loaded image in place, to an exact pixel target -
	// same math as read()'s resize step, just without needing to reload
	// from disk (e.g. to resize a crop() result).
	Img& resize(int width, int height, int interpolation = cv::INTER_AREA);

	// Loads an image from disk, optionally resizing it.
	// size = {0,0} means "keep original size".
	Img& read(const std::string& path,
		const std::pair<int, int>& size = {},
		bool keep_aspect = false,
		int interpolation = cv::INTER_AREA);

	// Draws this image onto other_img at pixel (x, y), alpha-blending if this image has a BGRA channel.
	void draw_on(Img& other_img, int x, int y);

	// Same placement as draw_on, but a plain pixel copy - no per-pixel alpha
	// math at all. Only correct when this image is known to be fully
	// opaque (e.g. the whole rendered board frame being placed onto a
	// fullscreen backdrop) - draw_on's alpha blend is wasted work on a
	// large, fully-opaque image (measurably slow at full-screen sizes,
	// since it splits/converts/re-merges every channel of every pixel even
	// though the result is identical to a plain copy).
	void draw_on_opaque(Img& other_img, int x, int y);

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
