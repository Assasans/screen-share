#include "Frame.hpp"

namespace ScreenShare {
	Frame::Frame() {
	}

	Frame::~Frame() {
	}

	CursorFrame::CursorFrame() {
	}

	CursorFrame::~CursorFrame() {
	}

	ScreenshotInfo::ScreenshotInfo() {
	}

	ScreenshotInfo::~ScreenshotInfo() {
	}

	cv::Size ScreenshotInfo::getSize() {
		return cv::Size(this->width, this->height);
	}

	CursorInfo::CursorInfo() {
	}

	CursorInfo::~CursorInfo() {
	}

	cv::Size CursorInfo::getSize() {
		return cv::Size(this->width, this->height);
	}
} // namespace ScreenShare
