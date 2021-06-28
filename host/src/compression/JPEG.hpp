#include <opencv2/opencv.hpp>

#include <turbojpeg.h>

#ifndef COMPRESSION_JPEG_H
#define COMPRESSION_JPEG_H

namespace ScreenShare::Host::Compression {
	class JPEG {
		public:
			JPEG(int width, int height);
			~JPEG();

			cv::Size size;

			void CompressFrame(cv::Mat image, std::vector<uint8_t>& compressed, int quality);
			void DecompressFrame(std::vector<uint8_t> compressed, cv::Mat& image);

		private:
			const int COLOR_COMPONENTS = 3;

			tjhandle _jpegCompressor;
			tjhandle _jpegDecompressor;

			unsigned long _jpegBufferSize = 0;
	};
} // namespace ScreenShare::Host::Compression

#endif
