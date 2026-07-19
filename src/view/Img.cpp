#include "Img.h"
#include <stdexcept>

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
	if (img.empty() || other_img.img.empty()) {
		throw std::runtime_error("Both images must be loaded before drawing.");
	}

	cv::Mat source_img = img;
	cv::Mat target_img = other_img.img;

	if (source_img.channels() != target_img.channels()) {
		if (source_img.channels() == 3 && target_img.channels() == 4) {
			cv::cvtColor(source_img, source_img, cv::COLOR_BGR2BGRA);
		}
		else if (source_img.channels() == 4 && target_img.channels() == 3) {
			cv::cvtColor(source_img, source_img, cv::COLOR_BGRA2BGR);
		}
	}

	int h = source_img.rows;
	int w = source_img.cols;
	int H = target_img.rows;
	int W = target_img.cols;

	if (y + h > H || x + w > W) {
		throw std::runtime_error("Image does not fit at the specified position.");
	}

	cv::Mat roi = target_img(cv::Rect(x, y, w, h));

	if (source_img.channels() == 4) {
		// Per-pixel alpha blend, channel by channel (not roi.col(c), which
		// selects a pixel *column* rather than a color channel and made
		// this crash via an accidental matrix multiply in cv::gemm).
		std::vector<cv::Mat> srcChannels;
		cv::split(source_img, srcChannels);

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
		source_img.copyTo(roi);
	}
}

void Img::draw_on_opaque(Img& other_img, int x, int y) {
	if (img.empty() || other_img.img.empty()) {
		throw std::runtime_error("Both images must be loaded before drawing.");
	}

	cv::Mat source_img = img;
	cv::Mat target_img = other_img.img;

	if (source_img.channels() != target_img.channels()) {
		if (source_img.channels() == 3 && target_img.channels() == 4) {
			cv::cvtColor(source_img, source_img, cv::COLOR_BGR2BGRA);
		}
		else if (source_img.channels() == 4 && target_img.channels() == 3) {
			cv::cvtColor(source_img, source_img, cv::COLOR_BGRA2BGR);
		}
	}

	int h = source_img.rows;
	int w = source_img.cols;
	int H = target_img.rows;
	int W = target_img.cols;

	if (y + h > H || x + w > W) {
		throw std::runtime_error("Image does not fit at the specified position.");
	}

	source_img.copyTo(target_img(cv::Rect(x, y, w, h)));
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

void Img::show() {
	if (img.empty()) {
		throw std::runtime_error("Image not loaded.");
	}

	cv::imshow("Image", img);
	cv::waitKey(0);
	cv::destroyAllWindows();
}

int Img::show(int waitMs) {
	if (img.empty()) {
		throw std::runtime_error("Image not loaded.");
	}

	cv::imshow("Image", img);
	return cv::waitKey(waitMs);
}
