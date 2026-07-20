#include "Img.h"
#include "../Constants.h"
#include <stdexcept>

namespace {
	// Validates both images are loaded, harmonizes channel counts (a BGR
	// source drawn onto a BGRA target or vice versa), and checks the source
	// fits at (x,y) within the target - shared by draw_on and
	// draw_on_opaque, which differ only in how they combine pixels
	// afterward (alpha blend vs. plain copy).
	cv::Mat prepareSourceForDraw(const cv::Mat& source, const cv::Mat& target, int x, int y) {
		if (source.empty() || target.empty()) {
			throw std::runtime_error("Both images must be loaded before drawing.");
		}

		cv::Mat preparedSource = source;
		if (preparedSource.channels() != target.channels()) {
			if (preparedSource.channels() == 3 && target.channels() == 4) {
				cv::cvtColor(preparedSource, preparedSource, cv::COLOR_BGR2BGRA);
			}
			else if (preparedSource.channels() == 4 && target.channels() == 3) {
				cv::cvtColor(preparedSource, preparedSource, cv::COLOR_BGRA2BGR);
			}
		}

		if (y + preparedSource.rows > target.rows || x + preparedSource.cols > target.cols) {
			throw std::runtime_error("Image does not fit at the specified position.");
		}

		return preparedSource;
	}
}

Img::Img() {}

Img Img::clone() const {
	Img result;
	result.img = img.clone();
	return result;
}

Img Img::crop(int x, int y, int w, int h) const {
	if (img.empty()) {
		throw std::runtime_error("Image not loaded.");
	}
	Img result;
	result.img = img(cv::Rect(x, y, w, h)).clone();
	return result;
}

Img& Img::resize(int width, int height, int interpolation) {
	if (img.empty()) {
		throw std::runtime_error("Image not loaded.");
	}
	cv::resize(img, img, cv::Size(width, height), 0, 0, interpolation);
	return *this;
}

Img Img::blank(int width, int height, const cv::Scalar& color) {
	// 4 channels (BGRA), matching board.png and every piece sprite, so
	// draw_on never hits its channel-mismatch fallback (which skips alpha
	// blending entirely and copies a hard rectangle instead).
	Img result;
	result.img = cv::Mat(height, width, CV_8UC4, color);
	return result;
}

Img& Img::read(const std::string& path,
	const std::pair<int, int>& size,
	bool keep_aspect,
	int interpolation) {
	img = cv::imread(path, cv::IMREAD_UNCHANGED);
	if (img.empty()) {
		throw std::runtime_error("Cannot load image: " + path);
	}

	if (size.first != 0 && size.second != 0) {
		int target_w = size.first;
		int target_h = size.second;
		int h = img.rows;
		int w = img.cols;

		if (keep_aspect) {
			double scale = std::min(static_cast<double>(target_w) / w,
				static_cast<double>(target_h) / h);
			int new_w = static_cast<int>(w * scale);
			int new_h = static_cast<int>(h * scale);
			cv::resize(img, img, cv::Size(new_w, new_h), 0, 0, interpolation);
		}
		else {
			cv::resize(img, img, cv::Size(target_w, target_h), 0, 0, interpolation);
		}
	}

	return *this;
}

void Img::draw_on(Img& other_img, int x, int y) {
	cv::Mat srcImg = prepareSourceForDraw(img, other_img.img, x, y);
	cv::Mat roi = other_img.img(cv::Rect(x, y, srcImg.cols, srcImg.rows));

	if (srcImg.channels() == 4) {
		// Per-pixel alpha blend, channel by channel (not roi.col(c), which
		// selects a pixel *column* rather than a color channel and made
		// this crash via an accidental matrix multiply in cv::gemm).
		std::vector<cv::Mat> srcChannels;
		cv::split(srcImg, srcChannels);

		// CV_32F (float, 4 bytes/pixel) instead of CV_64F (double, 8 bytes):
		// half the memory traffic for the same math, since 8-bit color
		// values never need double's extra precision. Measured as the
		// single biggest cost in the whole render pipeline before this.
		cv::Mat alpha;
		srcChannels[3].convertTo(alpha, CV_32F, 1.0 / 255.0);

		std::vector<cv::Mat> roiChannels;
		cv::split(roi, roiChannels);

		for (int c = 0; c < 3; ++c) {
			cv::Mat srcF, roiF, blendedF;
			srcChannels[c].convertTo(srcF, CV_32F);
			roiChannels[c].convertTo(roiF, CV_32F);

			blendedF = roiF.mul(1.0f - alpha) + srcF.mul(alpha);
			blendedF.convertTo(roiChannels[c], roiChannels[c].type());
		}
		if (roiChannels.size() == 4) {
			roiChannels[3].setTo(255);
		}

		cv::merge(roiChannels, roi);
	}
	else {
		srcImg.copyTo(roi);
	}
}

void Img::draw_on_opaque(Img& other_img, int x, int y) {
	cv::Mat srcImg = prepareSourceForDraw(img, other_img.img, x, y);
	srcImg.copyTo(other_img.img(cv::Rect(x, y, srcImg.cols, srcImg.rows)));
}

void Img::put_text(const std::string& txt, int x, int y, double font_size,
	const cv::Scalar& color, int thickness) {
	if (img.empty()) {
		throw std::runtime_error("Image not loaded.");
	}

	cv::putText(img, txt, cv::Point(x, y),
		cv::FONT_HERSHEY_SIMPLEX, font_size,
		color, thickness, cv::LINE_AA);
}

cv::Size Img::measureText(const std::string& txt, double font_size, int thickness) {
	int baseline = 0;
	return cv::getTextSize(txt, cv::FONT_HERSHEY_SIMPLEX, font_size, thickness, &baseline);
}

void Img::show() {
	if (img.empty()) {
		throw std::runtime_error("Image not loaded.");
	}

	cv::imshow(GAME_WINDOW_NAME, img);
	cv::waitKey(0);
	cv::destroyAllWindows();
}

int Img::show(int waitMs) {
	if (img.empty()) {
		throw std::runtime_error("Image not loaded.");
	}

	cv::imshow(GAME_WINDOW_NAME, img);
	return cv::waitKey(waitMs);
}
