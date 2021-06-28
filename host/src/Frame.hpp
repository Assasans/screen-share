#include <vector>
#include <cstdint>
#include <string>

#include <opencv2/opencv.hpp>

#ifndef FRAME_H
#define FRAME_H

namespace ScreenShare {
	class Frame {
		public:
			Frame();
			~Frame();



		private:
	};

	class CursorFrame {
		public:
			CursorFrame();
			~CursorFrame();



		private:
	};

	class ScreenshotInfo {
		public:
			ScreenshotInfo();
			~ScreenshotInfo();

			std::vector<uint64_t> pixels;

			int x;
			int y;

			int width;
			int height;

			int bpp;

			cv::Size getSize();

		private:
	};

	class CursorInfo {
		public:
			CursorInfo();
			~CursorInfo();

			std::string name;

			std::vector<uint64_t> pixels;

			int x;
			int y;

			int width;
			int height;

			cv::Size getSize();

		private:
	};
} // namespace ScreenShare

#endif
