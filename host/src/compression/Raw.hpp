#include <opencv2/opencv.hpp>

#ifndef COMPRESSION_RAW_H
#define COMPRESSION_RAW_H

namespace ScreenShare::Host::Compression {
	class Raw {
		public:
			Raw(int width, int height);
			~Raw();

			cv::Size size;

			void CompressFrame(cv::Mat image, std::vector<uint8_t>& compressed);
			void DecompressFrame(std::vector<uint8_t> compressed, cv::Mat& image);

		private:
			const int COLOR_COMPONENTS = 3;

			cv::Mat alphaChannel;
	};
} // namespace ScreenShare::Host::Compression

#endif
