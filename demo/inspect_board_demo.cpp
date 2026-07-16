// One-off diagnostic (not part of the game): scans board.png's middle row
// and column via Img::get_mat() to find where the decorative border ends
// and the actual 8x8 checkerboard begins, so Renderer can place pieces
// on the real grid instead of assuming it fills the whole image.
#include "../src/view/Img.h"
#include <iostream>

namespace {
	int channelAt(const cv::Mat& mat, int row, int col, int channel) {
		int channels = mat.channels();
		return mat.data[(row * mat.cols + col) * channels + channel];
	}
}

int main() {
	Img board;
	board.read(std::string(PROJECT_ROOT_DIR) + "/assets/board.png");
	const cv::Mat& mat = board.get_mat();

	std::cout << "size=" << mat.cols << "x" << mat.rows << " channels=" << mat.channels() << "\n";

	int midY = mat.rows / 2;
	int alphaIdx = mat.channels() - 1;
	std::cout << "-- middle row (y=" << midY << "), left edge, alpha channel --\n";
	for (int x = 0; x < 100; x += 1) {
		std::cout << x << ": alpha=" << channelAt(mat, midY, x, alphaIdx) << "\n";
	}
	std::cout << "-- middle row (y=" << midY << "), right edge, alpha channel --\n";
	for (int x = mat.cols - 100; x < mat.cols; x += 1) {
		std::cout << x << ": alpha=" << channelAt(mat, midY, x, alphaIdx) << "\n";
	}

	int midX = mat.cols / 2;
	std::cout << "-- middle col (x=" << midX << "), top edge, alpha channel --\n";
	for (int y = 0; y < 100; y += 1) {
		std::cout << y << ": alpha=" << channelAt(mat, y, midX, alphaIdx) << "\n";
	}

	return 0;
}
