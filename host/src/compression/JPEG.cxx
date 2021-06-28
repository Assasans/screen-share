#include "JPEG.hpp"

namespace ScreenShare::Host::Compression {
	JPEG::JPEG(int width, int height) {
		this->size = cv::Size(width, height);

		_jpegCompressor = tjInitCompress();
		_jpegDecompressor = tjInitDecompress();
	}

	JPEG::~JPEG() {
		_jpegCompressor = tjInitCompress();
		_jpegDecompressor = tjInitDecompress();
	}

	void JPEG::CompressFrame(cv::Mat image, std::vector<uint8_t>& compressed, int quality) {
		std::vector<uint8_t> buffer;

		uint8_t* _compressed = NULL;

		if(image.isContinuous()) {
			buffer.assign(image.data, image.data + image.total() * image.channels());
		} else {
			for(int i = 0; i < image.rows; ++i) {
				buffer.insert(buffer.end(), image.ptr<uint8_t>(i), image.ptr<uint8_t>(i) + image.cols * image.channels());
			}
		}

		unsigned long _jpegSize = _jpegBufferSize;
		unsigned long _oldJpegSize = _jpegSize;

		tjCompress2(
			_jpegCompressor,
			buffer.data(),
			image.cols,
			0,
			image.rows,
			TJPF_RGB,
			&_compressed,
			&_jpegSize,
			TJSAMP_444,
			quality,
			TJFLAG_FASTDCT
		);

		// printf("[Compress] %d | %d = %d\n", image.cols, image.rows, image.cols * image.rows * COLOR_COMPONENTS);

		// printf("Quality: %d | Raw: %d bytes | Compressed: %d bytes (size: %f%%)\n", quality, buffer.size(), _jpegSize, ((float)_jpegSize / buffer.size()) * 100);

		if(_jpegSize > _jpegBufferSize) {
			_jpegBufferSize = _jpegSize;

			printf("Extended JPEG size from %d bytes to %d bytes\n", _oldJpegSize, _jpegBufferSize);
		}

		compressed.resize(_jpegSize);

		memcpy(&compressed[0], _compressed, compressed.size());

		tjFree(_compressed);
	}

	void JPEG::DecompressFrame(std::vector<uint8_t> compressed, cv::Mat& image) {
		/*
		int width;
		int height;
		int subsamplimg;

		tjDecompressHeader2(
			_jpegDecompressor,
			compressed.data(),
			compressed.size(),
			&width,
			&height,
			&subsamplimg
		);
		*/

		// printf("[Decompress] %d | %d = %d\n", width, height, width * height * COLOR_COMPONENTS);

		uint8_t _decompressed[size.width * size.height * COLOR_COMPONENTS];

		tjDecompress2(
			_jpegDecompressor,
			compressed.data(),
			compressed.size(),
			_decompressed,
			size.width,
			0,
			size.height,
			TJPF_RGB,
			TJFLAG_FASTDCT
		);

		image = cv::Mat(size, CV_8UC3, _decompressed);
	}
} // namespace ScreenShare::Host::Compression
