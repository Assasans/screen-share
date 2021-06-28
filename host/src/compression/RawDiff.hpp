#include <opencv2/opencv.hpp>

#ifndef COMPRESSION_RAWDIFF_H
#define COMPRESSION_RAWDIFF_H

namespace ScreenShare::Host::Compression {
	class RawDiff {
		public:
			RawDiff();
			~RawDiff();

			void CompressFrame(cv::Mat image, std::vector<uint8_t>& compressed, int quality);
			void DecompressFrame(std::vector<uint8_t> compressed, cv::Mat& image);

		private:
			// TODO(Assasans): Multiple frames to cache
			cv::Mat previousFrame;
	};
} // namespace ScreenShare::Host::Compression

#endif
